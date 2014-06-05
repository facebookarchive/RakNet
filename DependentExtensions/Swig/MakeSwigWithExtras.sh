if ! type -P swig &>/dev/null
then
    echo "Swig is not installed want this script to download,make, and install it? ( y/n )"
    read answer
    if [ "$answer" = "n" ] || [ "$answer" = "N" ]
    then
	echo "Aborting"
	exit
    fi
 if [ ! -d "swig-2.0.0" ]
      then
             wget http://prdownloads.sourceforge.net/swig/swig-2.0.0.tar.gz
	     tar xzf swig-2.0.0.tar.gz 
	     cd swig-2.0.0
            ./configure 
	    make
            echo "Switching to root, enter root password"
	    su root -c "make install" #Automatically switches back after this command
	    echo "Exited root"
	    cd ../
    else
	    cd swig-2.0.0
            ./configure 
	    make
            echo "Switching to root, enter root password"
	    su root -c "make install" #Automatically switches back after this command
	    echo "Exited root"
	    cd ../
    fi
fi
if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ]
then
	echo "Invalid number of parameters, Usage: MakeSwigWithExtras.sh PATH_TO_RAKNETSOURCE PATH_TO_DEPENDENTEXTENSIONS OPTION1"
else
	echo "Performing Swig build"
	rm -f ./SwigOutput/SwigCSharpOutput/*
	if [ "$3" == "MYSQL_AUTOPATCHER" ]
	then
		echo "AutoPatcher MySql option added"
		swig -c++ -csharp -namespace RakNet -I"$1" -I"SwigInterfaceFiles" -I"$2" -DSWIG_ADDITIONAL_AUTOPATCHER_MYSQL -outdir SwigOutput/SwigCSharpOutput -o SwigOutput/CplusDLLIncludes/RakNet_wrap.cxx SwigInterfaceFiles/RakNet.i
		swigReturn=$?	
	else
		echo "Invalid option"
		swigReturn=1
	fi
	if [ $swigReturn == 0 ]
	then
		echo "Swig build complete"
		echo "Removing and replacing the sample cs files with fresh ones"
		mv ./SwigLinuxCSharpSample/TestMain.cs ./SwigLinuxCSharpSample/TestMain.txt 
		rm ./SwigLinuxCSharpSample/*.cs
		mv ./SwigLinuxCSharpSample/TestMain.txt ./SwigLinuxCSharpSample/TestMain.cs 
		cp ./SwigOutput/SwigCSharpOutput/*.cs ./SwigLinuxCSharpSample/
		echo "Building the Swig Dynamic Link"
		savedDir=`pwd`		
		cd $1
		if [ "$3" == "MYSQL_AUTOPATCHER" ]
		then
			echo "Building with AutoPatcher MySql"
             gcc -c $2/bzip2-1.0.3/blocksort.c $2/bzip2-1.0.3/bzip2.c $2/bzip2-1.0.3/bzlib.c $2/bzip2-1.0.3/compress.c $2/bzip2-1.0.3/crctable.c $2/bzip2-1.0.3/decompress.c $2/bzip2-1.0.3/dlltest.c $2/bzip2-1.0.3/huffman.c $2/bzip2-1.0.3/randtable.c
			 g++ *.cpp ../DependentExtensions/Swig/SwigOutput/CplusDLLIncludes/RakNet_wrap.cxx blocksort.o bzip2.o bzlib.o compress.o crctable.o decompress.o dlltest.o huffman.o randtable.o $2/Autopatcher/ApplyPatch.cpp $2/Autopatcher/AutopatcherClient.cpp $2/Autopatcher/AutopatcherMySQLRepository/AutopatcherMySQLRepository.cpp $2/Autopatcher/AutopatcherServer.cpp $2/Autopatcher/CreatePatch.cpp $2/Autopatcher/MemoryCompressor.cpp $2/MySQLInterface/MySQLInterface.cpp  -l pthread -lmysqlclient -I/usr/include/mysql/ -I./ -I$2/Autopatcher/AutopatcherMySQLRepository -I$2/Autopatcher -I$2/bzip2-1.0.3 -I$2/MySQLInterface -shared -o RakNet
			gccReturn=$?
		else
			echo "Invalid option" 
			gccReturn=1
		fi
		if [ $gccReturn == 0 ]
		then
			echo "Copying to /usr/lib/ will need root password"
		        if su root -c "cp ./RakNet /usr/lib" #Automatically switches back after this command
			then
				echo "RakNet lib copied"
			else
				echo "RakNet lib copy failed"
			fi
		        echo "Exited root"		
		else
			echo "There was an error durion the DLL build"
		fi
		cd $savedDir
	else
		echo "Swig had an error during build"
	fi
fi

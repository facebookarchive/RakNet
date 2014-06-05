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
if [ "$1" == "" ]
then
	echo "Invalid number of parameters, Usage: MakeSwig.sh directory"
else
	echo "Performing Swig build"
	rm -f ./SwigOutput/SwigCSharpOutput/*
	if [ "$2" == "" ]
	then
		echo "without SQLiteClientLogger"
		swig -c++ -csharp -namespace RakNet -I"$1" -I"SwigInterfaceFiles" -outdir SwigOutput/SwigCSharpOutput -o SwigOutput/CplusDLLIncludes/RakNet_wrap.cxx SwigInterfaceFiles/RakNet.i
	swigReturn=$?	
	else
		echo "with SQLiteClientLogger"
		swig -c++ -csharp -namespace RakNet -I"$1" -I"SwigInterfaceFiles" -I"$2" -DSWIG_ADDITIONAL_SQL_LITE -outdir SwigOutput/SwigCSharpOutput -o SwigOutput/CplusDLLIncludes/RakNet_wrap.cxx SwigInterfaceFiles/RakNet.i
	swigReturn=$?
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
		if [ "$2" == "" ]
		then
			echo "Building without SQLiteClientLogger"
			g++ *.cpp ../DependentExtensions/Swig/SwigOutput/CplusDLLIncludes/RakNet_wrap.cxx -l pthread -I./ -shared -o RakNet
		gccReturn=$?
		else
			echo "Building with SQLiteClientLogger"  
			g++ *.cpp ../DependentExtensions/Swig/SwigOutput/CplusDLLIncludes/RakNet_wrap.cxx "$2/SQLite3ClientPlugin.cpp"  "$2/SQLite3PluginCommon.cpp" "$2/Logger/ClientOnly/SQLiteClientLoggerPlugin.cpp"  "$2/Logger/SQLiteLoggerCommon.cpp"  -l pthread -I./ -I"$2/Logger/ClientOnly" -I"$2/Logger" -I"$2" -shared -o RakNet
		gccReturn=$?
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

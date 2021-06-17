The RakNet.sln solution contained under DLL_Swig is all you need to create the C# wrapper through SWIG. There are a few important steps.

You can find most of the steps in http://www.jenkinssoftware.com/raknet/manual/swigtutorial.html, but the instruction is incomplete, so here is what you need to do.

1. Download SWIG and extract it somewhere. Common location is under C:\ but anywhere will do.

2. Add your swig install directory to your PATH environment variable.

3. THIS IS IMPORTANT! Pre-create the output directories you need to use. The SWIG build process does not auto-generate output directories so it will fail if you don't do this.

You need to create 4 directories:
- RakNEt\DependentExtensions\Swig\SwigOutput
- RakNEt\DependentExtensions\Swig\SwigOutput\CplusDLLIncludes
- RakNEt\DependentExtensions\Swig\SwigOutput\SwigCSharpOutput
- RakNet\DependentExtensions\Swig\SwigWindowsCSharpSample\SwigTestApp\SwigFiles

4. Build the RakNet.sln file under RakNet\DependentExtensions\Swig\DLL_Swig

5. For Unity plugin, you need the .cs files from RakNet\DependentExtensions\Swig\SwigWindowsCSharpSample\SwigTestApp\SwigFiles
   and DLL file from RakNet\DependentExtensions\Swig\SwigWindowsCSharpSample\SwigTestApp\bin\X86\Release
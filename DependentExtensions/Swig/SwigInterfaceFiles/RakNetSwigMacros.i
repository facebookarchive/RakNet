//Macros that are not in a specific file are placed here, note that not all macros are here some are spread throughout the other files

//This you only need it once for all simple situations 
%define SIMPLE_OBJECT_OUTPUT_TYPEMAP(CTYPE, CSTYPE)
%typemap(ctype) CTYPE *OUTPUT, CTYPE &OUTPUT "CTYPE *"
%typemap(cstype) CTYPE *OUTPUT, CTYPE &OUTPUT "out CSTYPE"
%typemap(csin, 
         pre="    CSTYPE temp$csinput = new CSTYPE();", 
         post="      $csinput = temp$csinput;", 
         cshin="ref $csinput"
        )  CTYPE *OUTPUT, CTYPE &OUTPUT "CSTYPE.getCPtr(temp$csinput)"

%typemap(csdirectorin) CTYPE *OUTPUT, CTYPE &OUTPUT "$iminput"
%typemap(csdirectorout) CTYPE *OUTPUT, CTYPE &OUTPUT "$cscall"

%typemap(in) CTYPE *OUTPUT, CTYPE &OUTPUT
%{ $1 = ($1_ltype)$input; %}

%typemap(directorout,warning="Need to provide CTYPE *OUTPUT directorout typemap") TYPE *OUTPUT, TYPE &OUTPUT {
}

%typemap(directorin) CTYPE &OUTPUT
%{ $input = &$1; %}

%enddef
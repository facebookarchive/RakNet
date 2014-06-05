/// Apply \a patch to \a old.  Will return the new file in \a _new which is allocated for you.
bool ApplyPatch( char *old, unsigned int oldsize, char **_new, unsigned int *newsize, char *patch, unsigned int patchsize );


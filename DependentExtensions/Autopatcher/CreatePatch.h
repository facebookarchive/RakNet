/// Given \a old and \a new , return \a out which will contain a patch to get from \a old to \a new .  \a out is allocated for you.
bool CreatePatch(const char *old, unsigned oldsize, char *_new, unsigned int newsize, char **out, unsigned *outSize);


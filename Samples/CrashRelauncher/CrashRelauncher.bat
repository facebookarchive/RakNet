@echo off

if %1A==A goto error
if not %2n==n goto error

  :again
  start /b /wait %1
  goto again

goto end

:error
echo Enter the exe to run endlessly.

:end
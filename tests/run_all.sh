#! /bin/sh

for file in *.exe ; do
  echo -n Running $file...
  if $file ; then
      echo OK
  else
      echo Failed
  fi
done

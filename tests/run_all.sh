#! /bin/sh

for file in *.exe ; do
  echo -n Running $file...
  ./$file
done

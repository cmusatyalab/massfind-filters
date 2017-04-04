#!/bin/sh

set -e

if [ -z "$1" ] ; then
	echo "Usage: $0 <version>" >&2
	exit 1
fi

b="massfind-filters"

if [ "$1" = "dev" ] ; then
  v="HEAD"
else
  v="v$1"
fi

git archive --format tar "$v" "--prefix=$b-$1/" -o "$b-$1.tar"
gzip -9f "$b-$1.tar"

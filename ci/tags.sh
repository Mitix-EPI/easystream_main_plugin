#!/bin/sh

#Get the highest tag number
VERSION=`git describe --abbrev=0 --tags`
VERSION=${VERSION:-'0.0.0'}

#Get number parts
MAJOR="${VERSION%%.*}"; VERSION="${VERSION#*.}"
MINOR="${VERSION%%.*}"; VERSION="${VERSION#*.}"
PATCH="${VERSION%%.*}"; VERSION="${VERSION#*.}"

#Increase version
PATCH=$((PATCH+1))

#Create new tag
NEW_TAG="$MAJOR.$MINOR.$PATCH"
printf "%s" "$NEW_TAG"
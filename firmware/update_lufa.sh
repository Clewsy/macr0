#!/bin/bash

#Delete existing LUFA library if it exists.
rm -rf lib/LUFA

#Clone entire repo.
git clone https://github.com/abcminiuser/lufa/

#Only need the second-level LUFA directory.
mv lufa/LUFA lib/.

#Delete the remainder.
rm -rf lufa
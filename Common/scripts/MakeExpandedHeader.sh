#!/bin/bash
input_file=$1
header_list=$(grep '#include' $1| awk '{print $2}' | sed 's/[\"\<\>]//g' )
#echo $header_list

rootinc=$(root-config --incdir)
expand_option="-I${rootinc}"

## CMSSW options
if [[ -z $CMSS_BASE ]] ; then
  expand_option+=" -I."
else
  expand_option+=" -I${CMSSE_BASE}/src"
fi

for header in $(echo $header_list) ; do
  if [[ $(basename $header) == $(basename $input_file) ]] ; then
    continue;
  fi

  if [[ $header == T*.h ]] || [[ $header == ROOT* ]] || [[ $header == Roo*.h ]]; then
    expand_option+=" -imacros $header"
  elif [[ $header != *.* ]] ; then ## Standard library stuff
    expand_option+=" -imacros $header"
  elif [[ -f $header ]]; then ## Other stuff
    expand_option+=" -imacros $header"
  fi
done

tmp_file=/tmp/tmp.h
gcc ${expand_option} -E $input_file -o ${tmp_file} # Getting raw output
sed -i '/^#/d'    ${tmp_file}                       # Removing prepocessor tags
sed -i '/^\s*$/d' ${tmp_file}                       # Remove blank lines

first_line=$(grep -n "namespace.*usr" ${input_file} | head -n 1  | cut -f1 -d:)
total_line=$(wc -l ${input_file} | awk '{print $1}')
last_line=$(grep -n "^}" ${input_file} | tail -n 1 | cut -f1 -d:)

## Preparing output file
output_file=$(basename $input_file)
output_file=${output_file/.hpp/_stripped.hpp}

echo $output_file $first_line $last_line $total_line

head -n $((first_line-1)) $input_file > $output_file
cat ${tmp_file} >> $output_file
tail -n $((total_line-last_line)) $input_file >> $output_file

## Removing the temporary file
rm ${tmp_file}

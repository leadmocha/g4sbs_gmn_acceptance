#!/bin/bash

for dateCmd in generated may may_v2 july;
do
  kinematics/makeGMn.pl ${dateCmd}
done

#!/usr/bin/env python
# Software License Agreement (BSD License)
#
# Copyright (c) 2012, Philipp Wagner
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of the author nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

import sys, os, math, Image, glob,re

def Distance(p1,p2):
  dx = p2[0] - p1[0]
  dy = p2[1] - p1[1]
  return math.sqrt(dx*dx+dy*dy)

def ScaleRotateTranslate(image, angle, center = None, new_center = None, scale = None, resample=Image.BICUBIC):
  if (scale is None) and (center is None):
    return image.rotate(angle=angle, resample=resample)
  nx,ny = x,y = center
  sx=sy=1.0
  if new_center:
    (nx,ny) = new_center
  if scale:
    (sx,sy) = (scale, scale)
  cosine = math.cos(angle)
  sine = math.sin(angle)
  a = cosine/sx
  b = sine/sx
  c = x-nx*a-ny*b
  d = -sine/sy
  e = cosine/sy
  f = y-nx*d-ny*e
  return image.transform(image.size, Image.AFFINE, (a,b,c,d,e,f), resample=resample)

def CropFace(image, eye_left=(0,0), eye_right=(0,0), offset_pct=(0.2,0.2), dest_sz = (70,70)):
  # calculate offsets in original image
  offset_h = math.floor(float(offset_pct[0])*dest_sz[0])
  offset_v = math.floor(float(offset_pct[1])*dest_sz[1])
  # get the direction
  eye_direction = (eye_right[0] - eye_left[0], eye_right[1] - eye_left[1])
  # calc rotation angle in radians
  rotation = -math.atan2(float(eye_direction[1]),float(eye_direction[0]))
  # distance between them
  dist = Distance(eye_left, eye_right)
  # calculate the reference eye-width
  reference = dest_sz[0] - 2.0*offset_h
  # scale factor
  scale = float(dist)/float(reference)
  # rotate original around the left eye
  image = ScaleRotateTranslate(image, center=eye_left, angle=rotation)
  # crop the rotated image
  crop_xy = (eye_left[0] - scale*offset_h, eye_left[1] - scale*offset_v)
  crop_size = (dest_sz[0]*scale, dest_sz[1]*scale)
  image = image.crop((int(crop_xy[0]), int(crop_xy[1]), int(crop_xy[0]+crop_size[0]), int(crop_xy[1]+crop_size[1])))
  # resize it
  image = image.resize(dest_sz, Image.ANTIALIAS)
  return image

if __name__ == "__main__":
  #get all file *.jpg in current folder
  filelist = glob.glob('*.jpg')
  if len(filelist) == 0:
    print ('Not found any *.jpg file')
  else:
      while True:
        print('Found these *.jpg files')
        for n,i in enumerate(filelist):
          print("{0:5d}:{1}".format(n+1, i) )
        chosen = int(input("enter the NUMBER coressponding to the file you want to crop (0 to exit):"))
        if chosen == 0:
          break
        eye_left_x = int(input("enter the position of x of left_eye(0 to exit):"))
        eye_left_y = int(input("enter the position of y of left_eye(0 to exit):"))
        if eye_left_x == 0:
          break
        if eye_left_y == 0:
          break
        eye_right_x = int(input("enter the position of x of right_eye(0 to exit):"))
        eye_right_y = int(input("enter the position of y of right_eye(0 to exit):"))
        if eye_right_y == 0:
          break
        if eye_right_x == 0:
          break
        #select the filename from name lists
        filename = filelist[chosen-1] 

        # open image
        image = Image.open(filename)
        # get filename without extension
        mat = re.match('\w+[A-Za-z0-9]',filename)
        if mat:
          resultfile = mat.group(0)
        else:
          resultfile = filename
        
     #   CropFace(image, eye_left=(eye_left_x,eye_left_y), eye_right=(eye_right_x,eye_right_y), offset_pct=(0.1,0.1), dest_sz=(200,200)).save(filename+"_10_10_200_200.jpg")
     #   CropFace(image, eye_left=(eye_left_x,eye_left_y), eye_right=(eye_right_x,eye_right_y), offset_pct=(0.2,0.2), dest_sz=(200,200)).save(filename+"_20_20_200_200.jpg")
     #   CropFace(image, eye_left=(eye_left_x,eye_left_y), eye_right=(eye_right_x,eye_right_y), offset_pct=(0.3,0.3), dest_sz=(200,200)).save(filename+"_30_30_200_200.jpg")
        CropFace(image, eye_left=(eye_left_x,eye_left_y), eye_right=(eye_right_x,eye_right_y), offset_pct=(0.2,0.2)).save(resultfile+"_20_20_70_70.jpg")
        


       
       
        

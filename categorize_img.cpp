/*

   pHash, the open source perceptual hash library
   Copyright (C) 2009 Aetilius, Inc.
   All rights reserved.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Evan Klinger - eklinger@phash.org
   David Starkweather - dstarkweather@phash.org

*/

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <algorithm>
#include "pHash.h"

using namespace std;

#define TRUE 1
#define FALSE 0

//data structure for a hash and id
struct ph_imagepoint{
  ulong64 hash;
  char *id;
};

//aux function to create imagepoint data struct
struct ph_imagepoint* ph_malloc_imagepoint(){

  return (struct ph_imagepoint*)malloc(sizeof(struct ph_imagepoint));

}

//auxiliary function for sorting list of hashes
bool cmp_lt_imp(struct ph_imagepoint dpa, struct ph_imagepoint dpb){
  int result = strcmp(dpa.id, dpb.id);
  if (result < 0)
    return TRUE;
  return FALSE;
}


void handle(char** dir_name,
    struct dirent** dir_entry,
    vector<ph_imagepoint>* hashlist,
    ph_imagepoint** dp = NULL) {
  DIR *dir = opendir(*dir_name);
  if (!dir){
    printf("unable to open directory here\n");
    exit(1);
  }

  errno = 0;
  int i = 0;
  ulong64 tmphash;
  char path[100];
  path[0] = '\0';
  while ((*dir_entry = readdir(dir)) != 0){
    if (strcmp((*dir_entry)->d_name,".") && strcmp((*dir_entry)->d_name,"..")){
      strcat(path, *dir_name);
      strcat(path, "/");
      strcat(path, (*dir_entry)->d_name);
      if (ph_dct_imagehash(path, tmphash) < 0)  //calculate the hash
        continue;

      *dp = ph_malloc_imagepoint();              //store in structure with file name
      (*dp)->id = (*dir_entry)->d_name;
      (*dp)->hash = tmphash;
      (*hashlist).push_back(**dp);
      i++;
    }
    errno = 0;
    path[0]='\0';
  }

  if (errno){
    printf("error reading directory\n");
    exit(1);
  }

  sort((*hashlist).begin(),(*hashlist).end(),cmp_lt_imp);
}


/** TEST for image DCT hash function
 *  The program reads all images from the two directories given on the command line.
 *  The program expects the same number of image files in each directory. Each image
 *  should be perceptually similar to a corresponding file in the other directory and
 *  have the same exact name.  For example, one directory could contain the originals,
 *  and the other directory blurred counterparts.  The program calculates the hashes.
 *  First, the hamming distances are calculated between all similar image hashes (-i.e.
 *  the intra compares), and then hamming distances for different images hashes (-i.e.
 *  the inter compares).
 **/
int main(int argc, char **argv){

  const char *msg = ph_about();
  printf(" %s\n", msg);

  if (argc < 2){
    printf("no input args\n");
    printf("expected: \"test_imagephash [dir name]\"\n");
    exit(1);
  }

  // PRECOMPUTE HASHES FIRST.
  DIR *dir;
  struct dirent *ent;
  char *dir_name = (char*) malloc(sizeof(char) *100);
  if ((dir = opendir (argv[1])) != NULL) {
    // Iterate through every file in the directory.
    while ((ent = readdir (dir)) != NULL) {
      if (strcmp(ent->d_name,".") && strcmp(ent->d_name,"..")) {
        strcpy(dir_name, argv[1]);
        strcat(dir_name, "/");
        strcat(dir_name, ent->d_name);
        printf ("%s\n", dir_name);

        ph_imagepoint *dp = NULL;
        struct dirent *dir_entry;
        vector<ph_imagepoint> hashlist; //= new vector<ph_imagepoint>();
        handle(&dir_name, &dir_entry, &hashlist, &dp);
      }
    }
    closedir (dir);
  } else {
    printf("ERROR: could not open directory");
  }
  exit(1);

      ph_imagepoint *dp = NULL;
  char *dir_name1 = "./ref_images/ham"; //argv[1];
  char *dir_name2 = argv[2];
  struct dirent *dir_entry;
  vector<ph_imagepoint> hashlist1; //for hashes in first directory
  vector<ph_imagepoint> hashlist2; //for hashes in second directory

  //first directory
  handle(&dir_name1, &dir_entry, &hashlist1, &dp);

  //second directory
  handle(&dir_name2, &dir_entry, &hashlist2, &dp);

  int nbfiles1 = hashlist1.size();
  int nbfiles2 = hashlist2.size();

  int distance = -1;
/*  printf("**************************\n");
  printf("intra distance comparisons\n");
  printf("**************************\n");
  for (i=0;i<nbfiles;i++){
    printf(" %d %s %s ",i,hashlist1[i].id, hashlist2[i].id);

    //calculate distance
    distance = ph_hamming_distance(hashlist1[i].hash,hashlist2[i].hash);

    printf(" dist = %d\n",distance);
  }
*/

  printf("**************************\n");
  printf("inter distance comparisons\n");
  printf("**************************\n");
  for (int i=0;i<nbfiles1;i++){
    for (int j=0;j<nbfiles2;j++){
      printf(" %s %s ", hashlist1[i].id, hashlist2[j].id);

      //calculate distance
      distance = ph_hamming_distance(hashlist1[i].hash,hashlist2[j].hash);

      printf(" dist = %d\n",distance);
    }
    printf("\n");
  }
  printf("**************************\n");


  return 0;
}
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

  if (argc < 3){
    printf("no input args\n");
    printf("expected: \"test_imagephash [image dir] [ref dir]\"\n");
    printf("there should only be one image in [image dir]\n");
    exit(1);
  }

  printf("Computing hash for image.");
  ph_imagepoint *dp = NULL;
  char *image_dir_name = argv[1];
  struct dirent *dir_entry;
  vector<ph_imagepoint> image_hash_list;
  handle(&image_dir_name, &dir_entry, &image_hash_list, &dp);

  printf("Computing ref hashes.");
  DIR *dir;
  struct dirent *ent;
  char *dir_name = (char*) malloc(sizeof(char) *100);

  int shortest_distance = 50000;
  char *shortest_id;
  float shortest_average_distance = 50000;
  char *shortest_average_id;

  if ((dir = opendir (argv[2])) != NULL) {
    // Iterate through every file in the directory.
    while ((ent = readdir (dir)) != NULL) {
      if (strcmp(ent->d_name,".") && strcmp(ent->d_name,"..")) {
        strcpy(dir_name, argv[2]);
        strcat(dir_name, "/");
        strcat(dir_name, ent->d_name);
        // printf ("%s\n", dir_name);

        //struct dirent *dir_entry;
        vector<ph_imagepoint> hashlist; //= new vector<ph_imagepoint>();
        handle(&dir_name, &dir_entry, &hashlist, &dp);

        int distance = -1;
        int item_distance = 0;
        int num_items = 0;
        for (int i=0; i<hashlist.size(); i++){
          printf(" %s %s ", hashlist[i].id, image_hash_list[0].id);
          //calculate distance
          distance = ph_hamming_distance(hashlist[i].hash, image_hash_list[0].hash);
          printf(" dist = %d\n",distance);

          if (distance != 0) {
            item_distance += distance;
            num_items ++;
          }

          if (distance < shortest_distance && distance != 0) {
            shortest_distance = distance;
            shortest_id = hashlist[i].id;
          }
        }

        float average_distance = (float) item_distance / (float) num_items;
        printf("Average distance %f", average_distance);

        if (average_distance < shortest_average_distance) {
          shortest_average_distance = average_distance;
          shortest_average_id = hashlist[0].id;
        }

        printf("\n");
      }
    }

    printf("Shortest distance: %d\n", shortest_distance);
    printf("Shortest id: %s\n\n", shortest_id);

    printf("Shortest average distance: %f\n", shortest_average_distance);
    printf("Shortest average id: %s\n\n", shortest_average_id);

    closedir (dir);
  } else {
    printf("ERROR: could not open directory");
  }
  exit(1);

  return 0;
}

#include <world/worldgen/DefaultGen.h>
#include "world/worldgen/structures/TreeGen.h"
#include "world/worldgen/biomes/Biome.h"
#include "world/worldgen/structures/Pumpkins.h"
#include <sino/sino.h>
#include <stdio.h>
#include <time.h>
#include "misc/noise.h"
void DefaultGen_Init(DefaultGen* gen, World* world) { gen->world = world; }
int height = 0;
// based off https://github.com/smealum/3dscraft/blob/master/source/generation.c
void DefaultGen_Generate(WorkQueue* queue, WorkerItem item, void* this) {
	srand(time(NULL));
	enum Biomes biome = Biome_GetRandom();

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			
			float px = (float)(x + item.chunk->x * CHUNK_SIZE);
			float pz = (float)(z + item.chunk->z * CHUNK_SIZE);

			const int smeasClusterSize = 8;
			const int smeasChunkHeight = 16;
			height = (int)(sino_2d((px) / (smeasClusterSize * 4), (pz) / (smeasClusterSize * 4)) * smeasClusterSize) +
				(smeasChunkHeight * smeasClusterSize / 2);
			


			//for (int y = 0; y < height - 3; y++) {
			//	Chunk_SetBlock(item.chunk, x, y, z, Block_Stone);
			//}
			for (int y = 0; y < Biome_GetLevelInt(A, biome, height); y++)
			{
				Chunk_SetBlock(item.chunk, x, y, z, Biome_GetBlock(A, biome));
			}
			for (int y = Biome_GetLevelInt(A, biome, height); y < Biome_GetLevelInt(B, biome, height); y++)
			{
				Chunk_SetBlock(item.chunk, x, y, z, Biome_GetBlock(B, biome));
			}
			for (int y = Biome_GetLevelInt(B, biome, height); y < Biome_GetLevelInt(C, biome, height); y++)
			{
				Chunk_SetBlock(item.chunk, x, y, z, Biome_GetBlock(C, biome));
			}
			for (int y = Biome_GetLevelInt(C, biome, height); y < Biome_GetLevelInt(D, biome, height); y++)
			{
				Chunk_SetBlock(item.chunk, x, y, z, Biome_GetBlock(D, biome));
			}
			for (int y = Biome_GetLevelInt(D, biome, height); y < Biome_GetLevelInt(E, biome, height); y++)
			{
				Chunk_SetBlock(item.chunk, x, y, z, Biome_GetBlock(E, biome));
			}
			//for (int y = height - 3; y < height; y++) {
			//	Chunk_SetBlock(item.chunk, x, y, z, Block_Dirt);
			//}
			//Chunk_SetBlock(item.chunk, x, height, z, Block_Grass);
			for (int bd = 0; bd < 1 + rand() % 3; bd++)
            {Chunk_SetBlock(item.chunk, x, bd, z, Block_Bedrock);}
			//int l = rand() % 800;
            //if (l == 1){
			//	Pumpkins_Gen(queue, item);
			//}
			if (Biome_HasTrees(biome)){
			if (x == rand() % 17 && z == rand() % 17)
			{	
			
                if (x >= 4 && x <= 12){
                if (z >= 4 && z <= 12){
				
					TreeGen_GenTree(queue, item, x, height, z, GetTreeType(biome));
				
				
				
			}}}
			if (biome == Biome_Forest)
			{
			if (x == 4 + rand() % 8 && z == 4 + rand() % 8)
			{	
			
                if (x >= 4 && x <= 12){
                if (z >= 4 && z <= 12){
				
					TreeGen_GenTree(queue, item, x, height, z, GetTreeType(biome));
				
				
				
			}}}
			}}
		}
	}
	
}

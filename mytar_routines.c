#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied
 */
int copynFile(FILE * origin, FILE * destination, int nBytes){
	int cp = 0;
	char c;
	
	c = fgetc(origin);
	while(c != EOF && cp < nBytes){
		cp++;
		putc(c, destination);
		c = fgetc(origin);
	}


	return cp;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char* loadstr(FILE * file){
	// Complete the function
	// 1. contar bytes hasta leer \0 (tam)
	// 2. mover marcador de posicion tam bytes atras
	// 3. reservar tam bytes con malloc
	// 4. volver a leer los tam bytes escribiendolos en el buffer (acoraros de copiar el \0)
	// 5. devolver el buffer
	char *c;
	
	// 3
	c = malloc(tam*sizeof*char)); 
	return NULL;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry* readHeader(FILE * tarFile, int *nFiles){
	// Complete the function
	
	stHeaderEntry* headerArray = NULL;
	
	int nr_files = 0;
	int i = 0;
	
	// Leemos el numero de ficheros que tenemos
	if(fread(&nr_files, sizeof(int), 1, tarFile) < 1){
		printf("Error en la lectura de la cabecera del fichero .mytar. \n");
		return NULL;
	}
	
	headerArray = malloc(sizeof(stHeaderEntry)*nr_files);
	
	if(headerArray == NULL){
		printf("Error la reserva de memoria para la cabecera del fichero. \n");
		return NULL;
	}
	
	
	for(i = 0; i < nr_files; i++){
		
		printf("%d ---1\n", i);
		
		headerArray[i].name = loadstr(tarFile);
	
		printf("%d ---2\n", i);
		if( fread(&headerArray[i].size, sizeof(headerArray[i].size), 1, tarFile) < 1){
			printf("Error en la lectura del tamanio del archivo %d cabecera del fichero. \n", i);
			return NULL;
		}
		printf("%d ---3\n", i);
	}
	
	printf("200\n");
	
	(*nFiles) = nr_files;
	
	
	return headerArray;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[]) {

	FILE * tarFile;
	stHeaderEntry* headerArray = NULL;
	int sizeHeader;
	int sumLenFileName = 0;
	int fileSize = 0;
	int i = 0;
	int *pNfile;
	
	tarFile = fopen(tarName, "w");
	
	pNfile = malloc(sizeof(int));
	
	// Puntero a la primera posicion de la cabezera
	headerArray = malloc(sizeof(stHeaderEntry)*nFiles);
	
	if (headerArray == NULL) {
		perror("Error en la reserva de memoria de la cabecera del fichero mytar.\n");
		exit(EXIT_FAILURE);
	}
	
	// Tamanio de cabecera

	for(i = 0; i < nFiles; i++){
		sumLenFileName += strlen(fileNames[i]) + 1;
	}
	
	sizeHeader = sizeof(int) + sumLenFileName + sizeof(int)*nFiles;
	//sizeHeader = nFiles*sizeof(stHeaderEntry) + sizeof(int);
	
	// Nos posicionamos en el bytes tras la cabecera
	if(fseek( tarFile, sizeHeader, SEEK_SET ) == -1){
		printf("Error al posicionarnos con fseek en final de cabecera %s\n", tarName);
		exit(EXIT_FAILURE);
	}
	
	for(i = 0; i < nFiles; i++){
	
		fileSize = 0; //Reseteamos el valor de fileSize
		
		FILE * inputFile = fopen(fileNames[i], "r");
		
		// Calculamos el tamanio del fichero fileName
		fseek(inputFile, 0L, SEEK_END);
		if((fileSize = ftell(inputFile)) == -1){
			printf("Error al obtener tamanio del fichero: %s \n", fileNames[i]);
			exit(EXIT_FAILURE);
		};
		fseek(inputFile, 0L, SEEK_SET);
		
		if(copynFile(inputFile, tarFile, fileSize) < fileSize){
			printf("No hay galletas. \n");
			exit(EXIT_FAILURE);
		}
		

		if (fclose(inputFile) != 0) {
			printf("Error al cerrar el fichero %s \n", fileNames[i]);
			exit(EXIT_FAILURE);
		}
		
		headerArray[i].name = fileNames[i];
		headerArray[i].size = fileSize;
	}
	
	
	if(fseek(tarFile, 0, SEEK_SET) == -1){
		printf("Error posicionandose al principio del fichero  %s.mytar\n", tarName);
		exit(EXIT_FAILURE);
	
	}
	
	
	*pNfile = nFiles;
	
	fwrite(pNfile, sizeof(int), 1, tarFile);
	
	
	if(fwrite(headerArray, sizeof(stHeaderEntry), sizeof(headerArray), tarFile) < sizeof(headerArray) ){
		printf("Error al escribir en el fichero %s\n", tarName);
		exit(EXIT_FAILURE);
	}
	
	if (fclose(tarFile) != 0) {
		printf("Error al cerrar el fichero %s\n", tarName);
		exit(EXIT_FAILURE);
	}
	
	
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int extractTar(char tarName[]){
	// Complete the function
	FILE *tarFile = NULL;
	FILE *outputFile = NULL;
	int nFiles = 0;
	stHeaderEntry * headerArray = NULL;
	int i = 0;
	
	
	tarFile = fopen(tarName, "r");
	
	
	headerArray = readHeader(tarFile, nFiles);
	
	printf("LA TIENES PEQUENIA3\n");
	
	if(headerArray == NULL){
		exit(EXIT_FAILURE);
	}
	

	for(i = 0; i < nFiles; i++){
		// Creamos fichero con la informacion que hemos recibido en la cabecera stHeaderEntry
		if( (outputFile = fopen(headerArray[i].name, "w")) ==  NULL){
			printf("Error al abrir el fichero %s\n", headerArray[i].name);
			exit(EXIT_FAILURE);
		}
		
		copynFile(tarFile, outputFile, headerArray[i].size);
		
		if(fclose(outputFile) != 0){
			printf("Error al cerrar el fichero %s\n", headerArray[i].name);
			exit(EXIT_FAILURE);
		}
	}
	
	return EXIT_SUCCESS;
}


















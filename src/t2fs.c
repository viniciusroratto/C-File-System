
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/bitmap2.h"


#define ERRO -1
#define SUCESSO 0

#define MAX_ARQ 10
#define MAX_DIR 10

#define	END_OF_DIR	1





typedef int FILE2;
typedef int DIR2;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;

int current_entry = 0;

int fs_begin = 0 // flag de inicialização do FS.
char* cwd; // diretório corrente
DWORD cwdCluster;
DIRSTATUS* diretorios[MAX_DIR];
FILESTATUS arquivos[MAX_ARQ];
struct t2fs_superbloco* superbloco;


int ler_superbloco()
{
	BYTE buffer[SECTOR_SIZE];
	// em caso de falha na leitura.
	if(read_sector(0,(unsigned char *) &buffer) != 0){
		return ERRO;
	}
	else{
		superbloco = malloc(sizeof(unsigned char)*32);
		memcpy(superbloco,buffer,32);
		return SUCESSO;
	}
}


//Inicializa o FS.
// Boa prática é rodar o begin no início da cada uma das funções para garantir que o FS esteja inicializado.
//se já estiver inicializado retorna um sucesso, se não estiver inicializado, inicializa.
int begin()
{
	int i, j;

	if ( fs_begin == 0 )
	{
		ler_superbloco();
		cwd = malloc(sizeof(char)*MAX_FILE_NAME_SIZE);
		strcpy(cwd, "/");

		cwdCluster = superbloco->RootDirCluster; // Entender melhor estrutura de dados.
		
		for (i = 0; i < MAX_DIR; ++i)
		}
			if (diretorios[i] == NULL)
			{
			diretorios[i] = malloc(sizeof(FILESTATUS));
			diretorios[i]->ocupado=0;
			}
		}

		for(i=0; i < MAX_ARQ; j++)
		{
			if(arquivos == NULL)
			{
			arquivos[i] = malloc(sizeof(FILESTATUS));
			arquivos[i]->ocupado =0;
			}
		}
		
	
	}
	fs_begin = 1; 
	return SUCESSO;
}

/*-----------------------------------------------------------------------------
Função: Usada para identificar os desenvolvedores do T2FS.
	Essa função copia um string de identificação para o ponteiro indicado por "name".
	Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
	O string deve ser formado apenas por caracteres ASCII (Valores entre 0x20 e 0x7A) e terminado por ‘\0’.
	O string deve conter o nome e número do cartão dos participantes do grupo.

Entra:	name -> buffer onde colocar o string de identificação.
	size -> tamanho do buffer "name" (número máximo de bytes a serem copiados).

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size)
{
    char names[] = "Vinicius Roratto Carvalho - 00160094\n"
                    "Gabriel Tiburski Junior - 00229713\n"
                    "Txai Mostardeiro Potier - 00252858\n";

    if(size<sizeof(names))
        return ERRO;
    else
        strncpy(name,id, sizeof(id));
    return SUCESSO;
}

/*-----------------------------------------------------------------------------
A funçao seleciona o primeiro handle livre
Saída:	handle do arquivo ou sinal de erro.
        
-----------------------------------------------------------------------------*/
int getHandleArq()
{
	int i, max;
	max = MAX_ARQ;
	for (i=0; i<max; ++i) {
		if(arquivos[i]->ocupado == 0){
			return i;
		}
	}
	return ERRO;
}


void filenameParaArray(char *str,char *array[])
{
	init();
	int index = 0;
	array[index] = strtok(str,"/");
	while(array[index]!=NULL){
		array[++index] = strtok(NULL,"/");
	}
}


/*-----------------------------------------------------------------------------
Função:	A funçao calcula o indice do primeiro setor de um cluster
Entra:	nro_cluster -> número do cluster (indice do cluster,
                            mesmo tamanho de entradas na FAT)
Saída:	Retorna o numero do setor que corresponde ao primeiro setor do cluster
-----------------------------------------------------------------------------*/
DWORD buscaPrimeiroSetor(DWORD nro_cluster){
	init();
	DWORD n = nro_cluster*superbloco->SectorsPerCluster + superbloco->DataSectorStart;
	return n;
}


/**
 * Varre todas as entrada de diretorio do cluster a procura de um arquivo
 * @param nro_cluster numero do cluster a ser percorrido
 * @param nomeArquivo nome do arquivo a ser achado
 * @return numero do cluster de dados do arquivo
 *         -1, caso nao ter encontrado entrada com o nome do arquivo (0xFFFFFFFF)
 */
struct t2fs_record* busca_entrada_arquivo(DWORD nro_cluster, char *nomeArquivo){

	BYTE buffer[SECTOR_SIZE];
	DWORD primeiroSetor = buscaPrimeiroSetor(nro_cluster);
	struct t2fs_record* t2fsRecord = malloc(sizeof(struct t2fs_record)*4);
	int entradaDir =0;


	int i,j;
	for (i = 0; i < superbloco->SectorsPerCluster; ++i) {

		read_sector(primeiroSetor+i,(unsigned char *)buffer);
		memcpy(t2fsRecord,&buffer[0],SECTOR_SIZE);

		for (j = 0; j < 4; ++j) {
			//printf("    Entrada de diretorio %d: %s\n",entradaDir,t2fsRecord[j].name);
			if(strcmp(t2fsRecord[j].name,nomeArquivo)==0 && t2fsRecord[j].TypeVal==0x01){
				//printf("Encontrou arquivo com nome: %s\n",nomeArquivo);

				struct t2fs_record* retorno = malloc(sizeof(struct t2fs_record));
				memcpy(retorno,&t2fsRecord[j], sizeof(struct t2fs_record));
				return retorno;
			}
			entradaDir++;
		}
	}
	return NULL;
}

DWORD gravarRegistro(DWORD nro_cluster, struct t2fs_record *record){
	BYTE buffer[SECTOR_SIZE];
	DWORD primeiroSetor = getFirstSectorOfCluster(nro_cluster);
	struct t2fs_record* t2fsRecord = malloc(sizeof(struct t2fs_record)*4);
	int entradaDir =0;


	int i;
	int j;
	for (i = 0; i < superbloco->SectorsPerCluster; ++i) {

		read_sector(primeiroSetor+i,(unsigned char *)buffer);
		memcpy(t2fsRecord,&buffer[0],SECTOR_SIZE);

		for (j = 0; j < 4; ++j) {
			if(t2fsRecord[j].TypeVal==0x00){
			
				memcpy(&t2fsRecord[j],record, sizeof(struct t2fs_record));

				memcpy(&buffer[0],t2fsRecord,SECTOR_SIZE);

				if(write_sector(primeiroSetor+i,(unsigned char*)buffer)!=0){
	
					return ERRO;
				}
				return SUCESSO;
			}
			entradaDir++;
		}
	}
	return ERRO;
}



/*-----------------------------------------------------------------------------
Função: Criar um novo arquivo.
	O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
	O contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
	Caso já exista um arquivo ou diretório com o mesmo nome, a função deverá retornar um erro de criação.
	A função deve retornar o identificador (handle) do arquivo.
	Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.

Entra:	filename -> nome do arquivo a ser criado.

Saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo).
	Em caso de erro, deve ser retornado um valor negativo.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename)
{// verificar variaveis, e erros.
	begin();
	char* componente, path; 
	char* buffer_path = malloc(sizeof(char)*strlen(filename));
	int handle = getHandleArq();
	int qtd = 0;
	DWORD nro_cluster;
	DWORD diretorio_pai;
	struct t2fs_record record = {0};

	
	strcpy(buffer_path, filename);
	// caso de arquivo nulo ou vazio.
	if (filename = NULL || strlen(filename)==0)
		return ERRO; 

	if(handle < 0)
		return ERRO;

	while((componente = strsep(&path, "/")) != NULL)
	{
		if (strcmp(componente, "") == 0)
		}
			if (path == NULL)
				return ERRO;
			continue;
		}		
		qtd++;
		
		if (strlen(componente) > MAX_FILE_NAME_SIZE)
			return ERRO;
		else
			memcpy(record.name, componente, MAX_FILE_NAME_SIZE);

	}
	
	char *pathArray[qtd];
	filenameParaArray(filename, pathArray);
	
	nro_cluster = getNextFreeFatId();
	arquivos[handle]->ocupado=1;
	arquivos[handle]->cp=0;
	arquivos[handle]->type=0x01;
	arquivos[handle]->size=0;
	arquivos[handle]->firstCluster = nro_cluster;


	strcpy(arquivos[handle]->name, record.name);
	record.TypeVal-0x01;


	if (filename[0] == '/')
		diretorio_pai = superbloco->RootDirCluster;
	else
		diretorio_pai = cwdCluster;

	for(i=0; i < qtd-1; i++)
	{
		diretorio_pai = existDir(diretorio_pai, pathArray[i]);
		if(diretorio_pai==-1)
			return ERRO;
	}
	
	arquivos[handle]->clusterPai = diretorio_pai;
	
	if(busca_entrada_arquivo(diretorio_pai, pathArray[qtd -1])!= NULL)
		return ERRO;

	else
	{
		if(gravarRegistro(diretorio_pai, &record) == -1)
			return ERRO;
		lerFAT(nro_cluster);
		gravarFAT(clusterNunber, 0xFFFFFFFF);
		lerFAT(nro_cluster);
	}
	return handle;

}

int gravarFAT(int entry, int value){
	int sector, sectorOffset, byteOffset;
	BYTE sectorBuffer[SECTOR_SIZE];

	sectorOffset = entry/(SECTOR_SIZE/4);
	sector = superbloco->pFATSectorStart + sectorOffset;
	
	if(read_sector(sector, sectorBuffer) != 0){
		return ERRO;
	}
	
	byteOffset = (entry%(SECTOR_SIZE/4))*4;	//Calculates where in the sector is the entry
	memcpy(&sectorBuffer[byteOffset], &value, 4);	//Changes the entry
	
	//Writes the updated sector back to disk
	if(write_sector(sector, sectorBuffer) != 0){		
		//printf("Error writing FAT.\n");
		return ERRO;
	}
	
	return SUCESSO;
}



DWORD lerFAT(int entry){
	int sector, sectorOffset, byteOffset;
	BYTE sectorBuffer[SECTOR_SIZE];
	int content;
	
	sectorOffset = entry/(SECTOR_SIZE/4);	
	sector = superbloco->pFATSectorStart + sectorOffset;
	
	read_sector(sector, sectorBuffer);
	
	byteOffset = (entry%(SECTOR_SIZE/4))*4;
	memcpy(&content, &sectorBuffer[byteOffset], 4);
	return content;	
}


DWORD existDir(DWORD dirClusterNumber,char *filename)
{
	BYTE buffer[SECTOR_SIZE];
	DWORD primeiroSetor = buscaPrimeiroSetor(dirClusterNumber);
	struct t2fs_record* t2fsRecord = malloc(sizeof(struct t2fs_record)*4);
	int entradaDir =0;

	//printf("Verificando se exite diretorio %s no cluster: 0x%08X:\n",filename,dirClusterNumber);

	int i,j;
	for (i = 0; i < superbloco->SectorsPerCluster; ++i) {

		read_sector(primeiroSetor+i,(unsigned char *)buffer);
		memcpy(t2fsRecord,&buffer[0],SECTOR_SIZE);

		for (j = 0; j < 4; ++j) {
			//printf("    Entrada de diretorio %d: %s\n",entradaDir,t2fsRecord[j].name);
			if(strcmp(t2fsRecord[j].name,filename)==0 && t2fsRecord[j].TypeVal==0x02){

				/*printf("Achou diretorio chamado %s dentro do cluster 0x%08X\n",
					   t2fsRecord[j].name,
					   dirClusterNumber);*/

				//printf("Retornando record->firstCluster: 0x%08X\n\n",t2fsRecord[j].firstCluster);
				return t2fsRecord[j].firstCluster;
			}
			entradaDir++;
		}
	}
	return -1;

}



/*-----------------------------------------------------------------------------
Função:	Apagar um arquivo do disco.
	O nome do arquivo a ser apagado é aquele informado pelo parâmetro "filename".

Entra:	filename -> nome do arquivo a ser apagado.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int delete2 (char *filename)
{
begin();

	DWORD nro_cluster,ultimoClusterEncontrado;
	char* component;
	char* path;
	int qtd=0;

	path = malloc(sizeof(char)*strlen(filename));
	strcpy(path,filename);
	while ((component = strsep(&path, "/")) != NULL) {
		if (strcmp(component, "") == 0){
			if (path == NULL) {
				//printf("nao pode deletar arquivo sem nome1\n");
				return -1;
			}
			continue;
		}
		if(validaNome(component)==-1){
			return ERRO;
		}
		qtd+=1;
	}
	char *pathArray[qtd];

	filenameParaArray(filename,pathArray);

	if(qtd>0){

		if(filename[0]=='/'){
			//ler a partir da raiz
			nro_cluster = superbloco->RootDirCluster;

		}
		else{
			nro_cluster = cwdDirCluster;
		}

		ultimoClusterEncontrado = nro_cluster;
		int i;
		for (i = 0; i < qtd-1; ++i) {

			nro_cluster = existDir(ultimoClusterEncontrado,pathArray[i]);

			if(nro_cluster!=-1){
				ultimoClusterEncontrado = nro_cluster;
			}
			else{
				return -1;
			}
		}

		struct t2fs_record* fileRecord = busca_entrada_arquivo(ultimoClusterEncontrado, pathArray[qtd - 1]);

		if(fileRecord!=NULL && fileRecord->TypeVal!=0x00){

			DWORD fat_entry = fileRecord->firstCluster;
			DWORD fatEntryDel;

			//deletando entrada na FAT relacionado ao arquivo
			while(fat_entry >= 0x00000002 && fat_entry<=0xFFFFFFFD){
				//printf("FAT entry: 0x%08X\n",fat_entry);
				fatEntryDel = fat_entry;
				fat_entry = lerFAT(fatEntryDel);
				gravarFAT(fatEntryDel, 0x00000000);
				lerFAT(fatEntryDel);
			}

			deleteFileEntry(ultimoClusterEncontrado,fileRecord->name);

			return SUCESSO;
		}
		else{
			//nao foi possivel deletar: nao existe arquivo  ou arquivo invalido
			return ERRO;
		}
	}

	return ERRO;


}

int deleteFileEntry(DWORD nro_cluster,char* nomeArquivo)
{

	BYTE buffer[SECTOR_SIZE];
	DWORD primeiroSetor = buscaPrimeiroSetor(nro_cluster);
	struct t2fs_record* t2fsRecord = malloc(sizeof(struct t2fs_record)*4);
	int entradaDir =0;


	int i,j;
	for (i = 0; i < superbloco->SectorsPerCluster; ++i) {

		read_sector(primeiroSetor+i,(unsigned char *)buffer);
		memcpy(t2fsRecord,&buffer[0],SECTOR_SIZE);

		for (j = 0; j < 4; ++j) {
			
			if(strcmp(t2fsRecord[j].name,nomeArquivo)==0 && t2fsRecord[j].TypeVal==0x01){
		
				struct t2fs_record* record = malloc(sizeof(struct t2fs_record));
				record->TypeVal=0x00;
				record->bytesFileSize=0;
				record->firstCluster=0xFFFFFFFF;

				memcpy(&t2fsRecord[j],record, sizeof(struct t2fs_record));

				memcpy(&buffer[0],t2fsRecord,SECTOR_SIZE);

				if(write_sector(primeiroSetor+i,(unsigned char*)buffer)!=0){
					//printf("Error deleting entry\n");
					return -2;
				}
				//printf("Deletou arquivo com sucesso!\n");
				return 0;


			}
			entradaDir++;
		}
	}
	return ERRO;
}



/*-----------------------------------------------------------------------------
Função:	Abre um arquivo existente no disco.
	O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
	Ao abrir um arquivo, o contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
	A função deve retornar o identificador (handle) do arquivo.
	Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.
	Todos os arquivos abertos por esta chamada são abertos em leitura e em escrita.
	O ponto em que a leitura, ou escrita, será realizada é fornecido pelo valor current_pointer (ver função seek2).

Entra:	filename -> nome do arquivo a ser apagado.

Saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo)
	Em caso de erro, deve ser retornado um valor negativo
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename)
{
	begin();

	struct t2fs_record* record = existFileByPath(filename);

	if(record == NULL){
		//printf("erro ao abrir arquivo\n");
		return -1;
	}
	else{
		int handle = getHandleArq();
		if(handle!=-1){

			if(validaNome(record->name)==-1){
				return -32;
			}

			arquivos[handle]->ocupado=1;
			arquivos[handle]->cp=0;
			arquivos[handle]->type=record->TypeVal;
			arquivos[handle]->size=record->bytesFileSize;
			arquivos[handle]->firstCluster = record->firstCluster;
			strcpy(arquivos[handle]->name, record->name);
            arquivos[handle]->clusterPai = clusterArquivoRecemAberto;

			return handle;
		}
		else{
			//printf("Limite de arquivos abertos esgotado.\n");
			return -1;
		}
}

}


/*-----------------------------------------------------------------------------
Função:	Fecha o arquivo identificado pelo parâmetro "handle".

Entra:	handle -> identificador do arquivo a ser fechado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle)
{
	begin();

	if(handle<0 || handle>MAX_DIR){
		//tentando fechar handle fora do limite permitido (0-9)
		// para maximo de 10 arquivos aberto
		return -1;
	}
	if(arquivos[handle]->ocupado == 0){
		//tentando fechar arquivo livre
		return -1;
	}

	arquivos[handle]->ocupado=0;

    struct t2fs_record* record = malloc(sizeof(struct t2fs_record));

    record->bytesFileSize = arquivos[handle]->size;
    strcpy(record->name, arquivos[handle]->name);
    record->TypeVal = arquivos[handle]->type;
    record->firstCluster = arquivos[handle]->firstCluster;

    updateEntry(arquivos[handle]->clusterPai,record);

return 0;


}


/*-----------------------------------------------------------------------------
Função:	Realiza a leitura de "size" bytes do arquivo identificado por "handle".
	Os bytes lidos são colocados na área apontada por "buffer".
	Após a leitura, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último lido.

Entra:	handle -> identificador do arquivo a ser lido
	buffer -> buffer onde colocar os bytes lidos do arquivo
	size -> número de bytes a serem lidos

Saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes lidos.
	Se o valor retornado for menor do que "size", então o contador de posição atingiu o final do arquivo.
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size)
{
	FILESTATUS* file;
	BYTE *clusterBuffer = malloc(sizeof(BYTE) * (superbloco->SectorsPerCluster * SECTOR_SIZE));
	DWORD cluster, newCluster, firstSector;
	DWORD cpCluster, cpByte, cpInitial;
	int bytesWritten, bytesToWrite;
	int writeLoop, i;

	begin(); //Makes sure everything is initialized

	//Checks if the file is open
	if(checkIfOpen(handle)){
		file = arquivos[handle];
	}
	else{
		//printf("File is not open.\n");
		return -1;
	}

	//Initializes local variables
	writeLoop = 1;
	bytesWritten = 0;
	cpInitial = file->cp;	//Saves initial position of the CP
	cluster = file->firstCluster;	//File's first cluster
	cpCluster = file->cp / (superbloco->SectorsPerCluster * SECTOR_SIZE);	//File cluster CP is at
	//Navigates to the file cluster in the disk
	for(i = 0; i < cpCluster; i++){
		cluster = lerFAT(cluster);
	}
	
	while(writeLoop == 1){
		//Allocates a new cluster if needed
		if(cluster == -1){
			newCluster = getNextFreeFatID();
			gravarFAT(cluster, newCluster);
			gravarFAT(newCluster, 0xFFFFFFFF);
			cluster = newCluster;
		}

		firstSector = buscaPrimeiroSetor(cluster);	//Cluster's first sector
		cpByte = file->cp % (superbloco->SectorsPerCluster * SECTOR_SIZE);	//Byte in the cluster CP is at

		//Reads the current cluster from disk
		if(readCluster(firstSector, clusterBuffer) == -1){
			//printf("Error writing to file.\n");
			return -1;
		}

		//Calculate how much to write
		if((size - bytesWritten) < ((superbloco->SectorsPerCluster * SECTOR_SIZE) - cpByte)){
			bytesToWrite = size - bytesWritten;
		}
		else{
			bytesToWrite = (superbloco->SectorsPerCluster * SECTOR_SIZE) - cpByte;
		}

		//Writes cluster
		memcpy(&clusterBuffer[cpByte], &buffer[bytesWritten], bytesToWrite);
		if(writeCluster(firstSector, clusterBuffer) == -1){
			//printf("Error writing to file.\n");
			return -1;
		}

		bytesWritten += bytesToWrite;
		file->cp += bytesToWrite;

		if(bytesWritten == size){	//Done writing
			writeLoop = 0;
		}

		//Gets next cluster
		cluster = lerFAT(cluster);

	}

	if((cpInitial + bytesWritten) > file->size){
		file->size = file->cp;
	}

	return bytesWritten;



}


/*-----------------------------------------------------------------------------
Função:	Realiza a escrita de "size" bytes no arquivo identificado por "handle".
	Os bytes a serem escritos estão na área apontada por "buffer".
	Após a escrita, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último escrito.

Entra:	handle -> identificador do arquivo a ser escrito
	buffer -> buffer de onde pegar os bytes a serem escritos no arquivo
	size -> número de bytes a serem escritos

Saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes efetivamente escritos.
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size)
{
	FILESTATUS* file;
	BYTE *clusterBuffer = malloc(sizeof(BYTE) * (superbloco->SectorsPerCluster * SECTOR_SIZE));
	DWORD cluster, newCluster, firstSector;
	DWORD cpCluster, cpByte, cpInitial;
	int bytesWritten, bytesToWrite;
	int writeLoop, i;

	begin(); 

	//Checks if the file is open
	if(checkIfOpen(handle)){
		file = arquivos[handle];
	}
	else{
		//printf("File is not open.\n");
		return -1;
	}

	//Initializes local variables
	writeLoop = 1;
	bytesWritten = 0;
	cpInitial = file->cp;	//Saves initial position of the CP
	cluster = file->firstCluster;	//File's first cluster
	cpCluster = file->cp / (superbloco->SectorsPerCluster * SECTOR_SIZE);	//File cluster CP is at
	//Navigates to the file cluster in the disk
	for(i = 0; i < cpCluster; i++){
		cluster = lerFAT(cluster);
	}
	
	while(writeLoop == 1){
		//Allocates a new cluster if needed
		if(cluster == -1){
			newCluster = getNextFreeFatID();
			gravarFAT(cluster, newCluster);
			gravarFAT(newCluster, 0xFFFFFFFF);
			cluster = newCluster;
		}

		firstSector = buscaPrimeiroSetor(cluster);	//Cluster's first sector
		cpByte = file->cp % (superbloco->SectorsPerCluster * SECTOR_SIZE);	//Byte in the cluster CP is at

		//Reads the current cluster from disk
		if(readCluster(firstSector, clusterBuffer) == -1){
			//printf("Error writing to file.\n");
			return -1;
		}

		//Calculate how much to write
		if((size - bytesWritten) < ((superbloco->SectorsPerCluster * SECTOR_SIZE) - cpByte)){
			bytesToWrite = size - bytesWritten;
		}
		else{
			bytesToWrite = (superbloco->SectorsPerCluster * SECTOR_SIZE) - cpByte;
		}

		//Writes cluster
		memcpy(&clusterBuffer[cpByte], &buffer[bytesWritten], bytesToWrite);
		if(writeCluster(firstSector, clusterBuffer) == -1){
			//printf("Error writing to file.\n");
			return -1;
		}

		bytesWritten += bytesToWrite;
		file->cp += bytesToWrite;

		if(bytesWritten == size){	//Done writing
			writeLoop = 0;
		}

		//Gets next cluster
		cluster = lerFAT(cluster);

	}

	if((cpInitial + bytesWritten) > file->size){
		file->size = file->cp;
	}

	return bytesWritten;



}


/*-----------------------------------------------------------------------------
Função:	Função usada para truncar um arquivo.
	Remove do arquivo todos os bytes a partir da posição atual do contador de posição (CP)
	Todos os bytes a partir da posição CP (inclusive) serão removidos do arquivo.
	Após a operação, o arquivo deverá contar com CP bytes e o ponteiro estará no final do arquivo

Entra:	handle -> identificador do arquivo a ser truncado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int truncate2 (FILE2 handle)
{
	int fileCluster, i, cluster;
	FILESTATUS* file;
	
	begin();

	//Checks if the file is open
	if(checkIfOpen(handle)){
		file = arquivos[handle];
	}
	else{
		//printf("File is not open.\n");
		return -1;
	}
	
	fileCluster = file->cp/(superbloco->SectorsPerCluster*SECTOR_SIZE);	//Calculates cluster in file
	//Navigates to the current cluster in the disk
	cluster = file->firstCluster;
	for(i = 0; i < fileCluster; i++){
		cluster = lerFAT(cluster);
	}
	
	//Marks cluster as end of file in FAT
	if(gravarFAT(cluster, 0xFFFFFFFF) != 0){	
		//printf("Error truncating file.\n");
		return -1;
	}
	
	file->size = file->cp;	//Updates file size

	//printf("size truncate: %d",file->size);

	return 0;

}



/*-----------------------------------------------------------------------------
Função:	Reposiciona o contador de posições (current pointer) do arquivo identificado por "handle".
	A nova posição é determinada pelo parâmetro "offset".
	O parâmetro "offset" corresponde ao deslocamento, em bytes, contados a partir do início do arquivo.
	Se o valor de "offset" for "-1", o current_pointer deverá ser posicionado no byte seguinte ao final do arquivo,
		Isso é útil para permitir que novos dados sejam adicionados no final de um arquivo já existente.

Entra:	handle -> identificador do arquivo a ser escrito
	offset -> deslocamento, em bytes, onde posicionar o "current pointer".

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, DWORD offset)
{
	FILESTATUS* file;

	begin();

	//Checks if the file is open
	if(checkIfOpen(handle)){
		file = arquivos[handle];
	}
	else{
		//printf("File is not open.\n");
		return -1;
	}

	if(offset == 0xFFFFFFFF){
		file->cp = file->size;	//1 byte after the end of the file
		//printf("CP: %d\n",arquivos[handle]->cp);
		return 0;
	}

	if(offset < 0xF0000000 && offset > file->size){	//Offset is past the file area
		//printf("Offset is too big.\n");
		return -1;
	}

	if(offset > 0xF0000000){
		//printf("Invalid offset value.\n");
		return -1;
	}
	else {
		file->cp = offset;
		//printf("CP: %d\n", arquivos[handle]->cp);

		return 0;
}
}


/*-----------------------------------------------------------------------------
Função:	Criar um novo diretório.
	O caminho desse novo diretório é aquele informado pelo parâmetro "pathname".
		O caminho pode ser ser absoluto ou relativo.
	São considerados erros de criação quaisquer situações em que o diretório não possa ser criado.
		Isso inclui a existência de um arquivo ou diretório com o mesmo "pathname".

Entra:	pathname -> caminho do diretório a ser criado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname)
{
	begin(); 
	DWORD parentCluster, newClusterDir;

	struct t2fs_record* newDirEntry = malloc(sizeof(struct t2fs_record));
	char* pathBuffer;
	int pathLevels, i;

	pathBuffer = malloc(sizeof(char)*(strlen(cwd)+strlen(pathname)));

	strcpy(pathBuffer,pathname);
	pathLevels = countPathLevels(pathBuffer);
    	char *pathArray[pathLevels];
    	filenameParaArray(pathBuffer, pathArray);

	if(validaNome(pathArray[pathLevels-1])==-1){
		return -32;
	}

	//Sets up the new entry
    	newDirEntry->TypeVal = 0x02;
    	strcpy(newDirEntry->name, pathArray[pathLevels-1]);
    	newDirEntry->bytesFileSize = SECTOR_SIZE * superbloco->SectorsPerCluster;
	newDirEntry->firstCluster = getNextFreeFatID();


	if(pathname[0]=='/'){
		//printf("criar diretorio a partir da raiz\n");
		parentCluster = superbloco->RootDirCluster;

	}
	else{
		//printf("Criar diretorio a partir do diretorio corrente\n");
		parentCluster = cwdDirCluster;
	}

	if(pathLevels == 1){
		//Devemos criar diretorio na raiz
        if(existDir(parentCluster, pathArray[pathLevels-1]) != -1){
        	//printf("Directory already exists.\n");
        	return -1;
        }

        if(gravarRegistro(parentCluster, newDirEntry) == -1){
            //printf("Parent directory is full.\n");
            return -1;
        }

        gravarFAT(newDirEntry->firstCluster, 0xFFFFFFFF);
    }
    else {
        for (i = 0; i < pathLevels - 1; i++) {

			if(validaNome(pathArray[i])==-1){
				return -32;
			}
            parentCluster = existDir(parentCluster, pathArray[i]);
            if(parentCluster == -1){
                //printf("Parent directory does not exist.\n");
                return -1;
            }
        }

        if(existDir(parentCluster, pathArray[pathLevels-1]) != -1){
        	//printf("Directory already exists.\n");
        	return -1;
        }

        if(gravarRegistro(parentCluster, newDirEntry) == -1){
            //printf("Parent directory is full.\n");
            return -1;
        }

        gravarFAT(newDirEntry->firstCluster, 0xFFFFFFFF);
    }

    newClusterDir = newDirEntry->firstCluster;

	eraseClusterContent(newDirEntry->firstCluster);

    //Sets up the . entry
    newDirEntry->TypeVal = 0x02;
    strcpy(newDirEntry->name, ".");
    newDirEntry->bytesFileSize = SECTOR_SIZE * superbloco->SectorsPerCluster;
    newDirEntry->firstCluster = newClusterDir;

    if(gravarRegistro(newClusterDir, newDirEntry) == -1){
        //printf("Error creating . entry.\n");
        return -1;
    }

    //Sets up the .. entry
    newDirEntry->TypeVal = 0x02;
    strcpy(newDirEntry->name, "..");
    newDirEntry->bytesFileSize = SECTOR_SIZE * superbloco->SectorsPerCluster;
    //printf("Cluster pai 0x%08X,",parentCluster);
    newDirEntry->firstCluster = parentCluster;

    if(gravarRegistro(newClusterDir, newDirEntry)){
        //printf("Error creating .. entry.\n");
        return -1;
    }

return 0;

}


/*-----------------------------------------------------------------------------
Função:	Apagar um subdiretório do disco.
	O caminho do diretório a ser apagado é aquele informado pelo parâmetro "pathname".
	São considerados erros quaisquer situações que impeçam a operação.
		Isso inclui:
			(a) o diretório a ser removido não está vazio;
			(b) "pathname" não existente;
			(c) algum dos componentes do "pathname" não existe (caminho inválido);
			(d) o "pathname" indicado não é um arquivo;

Entra:	pathname -> caminho do diretório a ser criado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname)
{
	begin();

	char* component;
	char* path;
	int qtd=0;

	path = malloc(sizeof(char)*strlen(pathname));
	strcpy(path,pathname);//copia o path

	while ((component = strsep(&path, "/")) != NULL) {
		if (strcmp(component, "") == 0){
			if (path == NULL) {
				//printf("nao pode deletar diretorio sem nome1\n");
				return -1;
			}
			continue;
		}
		if(validaNome(component)==-1){
			return -32;
		}
		qtd+=1;
	}

	char *pathArray[qtd];
	filenameParaArray(pathname,pathArray);

	//printf("cwdDir: 0x%08x rootdir: 0x%08x\n",cwdDirCluster,superbloco->RootDirCluster);

	DWORD clusterPai;
	DWORD clusterFilho;

	if(pathname[0]=='/'){
		clusterPai = superbloco->RootDirCluster;
	}
	else{
		clusterPai = cwdDirCluster;
	}

	int i;
	for (i = 0; i < qtd - 1; ++i) {

		clusterPai = existDir(clusterPai, pathArray[i]);
		if (clusterPai == -1) {
			//printf("Não existe diretório chamado: %s", pathArray[i]);
		}
	}


	//printf("Cluster pai: 0x%08x\n",clusterPai);
	//printf("queremos deletar o dir: %s\n",pathArray[qtd-1]);

	//verifica se existe diretório a ser deletado
	clusterFilho = existDir(clusterPai,pathArray[qtd-1]);

	//printf("Cluster filho: 0x%08x\n",clusterFilho);

	if(clusterFilho != -1){

		//Sets up the new entry
		struct t2fs_record newDirEntry;
		newDirEntry.TypeVal = 0x00;
		newDirEntry.bytesFileSize = 0;
		newDirEntry.firstCluster = 0;


		strcpy(newDirEntry.name,pathArray[qtd-1]);

		if(gravarFAT(clusterFilho, 0x00000000) == -1){
			//printf("Error removing directory.\n");
			return -1;
		}

		if(updateEntry(clusterPai, &newDirEntry) == -1){
			//printf("Error removing directory.\n");
			return -1;
		}

		BYTE emptyCluster[SECTOR_SIZE * superbloco->SectorsPerCluster];

		//Initializes emptyCluster
		//printf("sizeof: %d\n", sizeof(emptyCluster));
		int i;
		for(i = 0; i < sizeof(emptyCluster); i++){
			emptyCluster[i] = 0;
		}

		//eraseClusterContent(clusterFilho);

		return 0;
	}
	else{
		//printf("Error, is %s a directory?\n",pathArray[qtd-1]);
		return -1;
	}

	return -1;

}


/*-----------------------------------------------------------------------------
Função:	Altera o diretório atual de trabalho (working directory).
		O caminho desse diretório é informado no parâmetro "pathname".
		São considerados erros:
			(a) qualquer situação que impeça a realização da operação
			(b) não existência do "pathname" informado.

Entra:	pathname -> caminho do novo diretório de trabalho.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int chdir2 (char *pathname)
{
	begin();


	DWORD cluster;
	char* pathBuffer = malloc(sizeof(char)*strlen(pathname));
	char* absolutePath = malloc(sizeof(char)*200);
	int pathLevels, i;

	buildAbsolutePath(pathname, absolutePath);
	strcpy(pathBuffer, pathname);

	pathLevels = countPathLevels(pathBuffer);

	char *pathArray[pathLevels];
	filenameParaArray(pathBuffer, pathArray);

	if(pathname[0]=='/'){
		//printf("absoluto 0x%08X, %s\n",superbloco->RootDirCluster,pathname);
		cluster = superbloco->RootDirCluster;
	}
	else{
		//o cluster usado deve ser o do diretorio corrente
		//printf("relativo 0x%08x, %s\n",cwdDirCluster,pathname);
		cluster = cwdDirCluster;
	}

	for (i = 0; i < pathLevels; i++) {

		if(validaNome(pathArray[i])==-1){
			return -32;
		}
		cluster = existDir(cluster, pathArray[i]);
		//printf("cluster dentro do for: 0x%08X\n",cluster);
		if(cluster == -1){
			//printf("Directory does not exist.\n");
			return -1;
		}
	}


	parseDots(absolutePath, cwd);
	cwdDirCluster = cluster;
	//printf("Atualizou cluster do diretorio corrente: 0x%08X\n",cwdDirCluster);
	return 0;

}


/*-----------------------------------------------------------------------------
Função:	Informa o diretório atual de trabalho.
		O "pathname" do diretório de trabalho deve ser copiado para o buffer indicado por "pathname".
			Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
		São considerados erros:
			(a) quaisquer situações que impeçam a realização da operação
			(b) espaço insuficiente no buffer "pathname", cujo tamanho está informado por "size".

Entra:	pathname -> buffer para onde copiar o pathname do diretório de trabalho
		size -> tamanho do buffer pathname

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int getcwd2 (char *pathname, int size)
{

	begin();
	//Size is not big enough
	if(size < sizeof(cwd)+1){
		//printf("Buffer too small.\n");
		return -1;
	}

	strcpy(pathname,cwd);

	return 0;

}


/*-----------------------------------------------------------------------------
Função:	Abre um diretório existente no disco.
	O caminho desse diretório é aquele informado pelo parâmetro "pathname".
	Se a operação foi realizada com sucesso, a função:
		(a) deve retornar o identificador (handle) do diretório
		(b) deve posicionar o ponteiro de entradas (current entry) na primeira posição válida do diretório "pathname".
	O handle retornado será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do diretório.

Entra:	pathname -> caminho do diretório a ser aberto

Saída:	Se a operação foi realizada com sucesso, a função retorna o identificador do diretório (handle).
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname);
{
	begin();
	//printf("opendir2()\n");

	int handle = getFreeDirHandle(diretorios);

	if(handle < 0){
		//numero maximo de diretorios aberto foi alcançado
		return -1;
	}

	DWORD firstCluster;

	char *dir = strtok(pathname,"/");
	char *nome;

	if(pathname[0]=='/'){
		//printf("Open absoluto\n");
		firstCluster = superbloco->RootDirCluster;
	}
	else{
		//printf("Open relativo\n");
		firstCluster = cwdDirCluster;
	}


	if(dir == NULL){
		//Se for null usuario passou '/' (diretorio raiz)
		//diretorios[handle] = malloc(sizeof(struct DIRSTATUS));
		//return handle;
		//printf("Foi passado somente /\n");

		diretorios[handle] = malloc(sizeof(struct DIRSTATUS));
		diretorios[handle]->hDir = malloc(sizeof(DIRENT2));

		diretorios[handle]->ocupado =1;
		strcpy(diretorios[handle]->hDir->name,"/");
		diretorios[handle]->hDir->fileType=0x02;
		diretorios[handle]->hDir->fileSize=superbloco->SectorsPerCluster*SECTOR_SIZE;
		diretorios[handle]->firstCluster = firstCluster;

		return handle;
	}
	else{
		//percorre caminho relativo

		DWORD d;
		while (dir!=NULL) {

			if(validaNome(dir)==-1){
				return -32;
			}
			d = existDir(firstCluster,dir);
			if(d!=-1){
				firstCluster = d;
				nome = dir;
			}
			else{
				return -1;
			}

			dir = strtok(NULL,"/");
		}



		diretorios[handle] = malloc(sizeof(struct DIRSTATUS));
		diretorios[handle]->hDir = malloc(sizeof(DIRENT2));

		diretorios[handle]->ocupado =1;
		strcpy(diretorios[handle]->hDir->name,nome);
		diretorios[handle]->hDir->fileType=0x02;
		diretorios[handle]->hDir->fileSize=superbloco->SectorsPerCluster*SECTOR_SIZE;
		diretorios[handle]->firstCluster = firstCluster;

		/*printf("-->Handle: %d\n",handle);
        printf("-->Handle ocupado: %d\n",diretorios[handle]->ocupado);
        printf("-->Nome do diretorio atrelado ao handle: %s\n",diretorios[handle]->hDir->name);
        printf("-->typeVal: 0x%02X\n",diretorios[handle]->hDir->fileType);
        printf("-->Size: %d\n",diretorios[handle]->hDir->fileSize);*/

		return handle;

	}
{

/*-----------------------------------------------------------------------------
Função:	Realiza a leitura das entradas do diretório identificado por "handle".
	A cada chamada da função é lida a entrada seguinte do diretório representado pelo identificador "handle".
	Algumas das informações dessas entradas devem ser colocadas no parâmetro "dentry".
	Após realizada a leitura de uma entrada, o ponteiro de entradas (current entry) deve ser ajustado para a próxima entrada válida, seguinte à última lida.
	São considerados erros:
		(a) qualquer situação que impeça a realização da operação
		(b) término das entradas válidas do diretório identificado por "handle".

Entra:	handle -> identificador do diretório cujas entradas deseja-se ler.
	dentry -> estrutura de dados onde a função coloca as informações da entrada lida.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero ( e "dentry" não será válido)
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry)
{
	begin();

	if(handle<0 || handle>MAX_DIR){
		return -2;
	}
	if(diretorios[handle]->ocupado == 0){
		return -3;
	}

	//printf("handle %d, firstcluster: 0x%08X\n",handle,diretorios[handle]->firstCluster);

	DWORD primeiroSetor = buscaPrimeiroSetor(diretorios[handle]->firstCluster);
	BYTE buffer[SECTOR_SIZE*superbloco->SectorsPerCluster];
	struct t2fs_record* t2fsRecord = malloc(sizeof(struct t2fs_record)*16);
	int i;
	for (i = 0; i <superbloco->SectorsPerCluster ; ++i) {
		read_sector(primeiroSetor+i,(unsigned char *)buffer);
		memcpy(&t2fsRecord[i*4],&buffer[0],SECTOR_SIZE);
	}

	while(!(t2fsRecord[current_entry].TypeVal==0x01
			|| t2fsRecord[current_entry].TypeVal==0x02) && current_entry<16) {

		current_entry++;
	}
	if(current_entry<16){
		dentry->fileSize = t2fsRecord[current_entry].bytesFileSize;
		dentry->fileType = t2fsRecord[current_entry].TypeVal;
		strcpy(dentry->name, t2fsRecord[current_entry].name);
		current_entry++;
		return 0;
	}
	else{
		current_entry=0;
		return -END_OF_DIR;
	}
}


/*-----------------------------------------------------------------------------
Função:	Fecha o diretório identificado pelo parâmetro "handle".

Entra:	handle -> identificador do diretório que se deseja fechar (encerrar a operação).

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle)
{
	begin();

	if(handle<0 || handle> MAX_DIR){
		return ERRO;
	}
	if(diretorios[handle]->ocupado == 0){
		return ERRO;
	}


	diretorios[handle]->ocupado=0;
	diretorios[handle]->hDir=NULL;
	return SUCESSO;
}


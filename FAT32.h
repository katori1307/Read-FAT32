#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include "Object.h"
using namespace std;

class FAT32
{
private:

	LPCWSTR drive; // kiểu dữ liệu const để lưu đường dẫn đến ổ đĩa
	//các thông số cần thiết
	unsigned int byte_per_sector;
	unsigned int sector_per_cluster;
	unsigned int sector_in_bootsector;
	unsigned int num_of_FAT;
	unsigned int sector_in_RDET;
	unsigned int sector_per_FAT;
	unsigned int first_RDET_sector;
	unsigned int first_DATA_sector;

	BYTE* FAT; // để lưu dữ liệu bảng FAT
	//BYTE* RDET; // để lưu dữ liệu bảng RDET
	BYTE* BOOTSECTOR; // để lưu dữ liệu bootsector
	
	vector<Object*>list;

public:
	FAT32();
	FAT32(LPCWSTR drive);
	~FAT32();
	
	//Hàm đọc dữ liệu của sector (bootsector, FAT, RDET ... ) n byte tại vị trí offset theo định dạng little-endian
	static unsigned int get_value_from_sector(BYTE* sector, int offset, int byte);
	
	//Hàm đọc dữ liệu của sector được chỉ định từ vị trí readPoint với kích thước là size
	int readSector(int readPoint, BYTE*& sector, int size);

	//Hàm in bảng sector.
	static void print_sector(BYTE* sector);

	//hàm chuyển từ hexa sang string
	static string getString(BYTE* sector, int offset, int byte);
	
	//hàm đọc cây thư mục gốc, biến depth là độ sâu của 1 tập tin/ thư mục
	void readRDET(int sector, int depth);

	//hàm in thông tin file/directory
	static void showInfo(string name, string status, unsigned int size, unsigned int firstCluster, unsigned int lastCluster);

	//hàm in thông tin trong RDET
	void print_RDET();

	//in thông tin bootsector
	void print_bootsector();

	//in cây thư mục
	void print_directory_tree();


	void read_FAT(BYTE* FAT, int startPoint);
	// hàm in các sector mà thư mục/ tập tin chiếm giữ
	void get_used_sector(int firstCluser,int lastCluster, int firstDataSector, int sectorPerCluster);

	string read_text_file(int firstCluster);

	void print_file_content(int sector);
};
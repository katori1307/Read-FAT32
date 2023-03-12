#include "FAT32.h"
#include "Object.h"
FAT32::FAT32()
{
	this->drive = L"////.//F:";

    this->FAT = new BYTE[512];
    this->BOOTSECTOR = new BYTE[512];

    this->readSector(0, this->BOOTSECTOR, 512);

    this->byte_per_sector = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x0B, 2);
    this->sector_per_cluster = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x0D, 1);
    this->sector_in_bootsector = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x0E, 2);
    this->num_of_FAT = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x10, 1);
    this->sector_in_RDET = (FAT32::get_value_from_sector(this->BOOTSECTOR, 0x11, 2) * 32) / 512;
    this->sector_per_FAT = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x24, 4);
    this->first_DATA_sector = this->sector_in_bootsector + this->num_of_FAT * this->sector_per_FAT;
    this->first_RDET_sector = this->sector_in_bootsector + this->num_of_FAT * this->sector_per_FAT + this->sector_in_RDET;
}

FAT32::FAT32(LPCWSTR drive)
{
    this->drive = drive;

    this->FAT = new BYTE[512];
    this->BOOTSECTOR = new BYTE[512];

    this->readSector(0, this->BOOTSECTOR, 512);

    this->byte_per_sector = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x0B, 2);
    this->sector_per_cluster = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x0D, 1);
    this->sector_in_bootsector = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x0E, 2);
    this->num_of_FAT = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x10, 1);
    this->sector_in_RDET = (FAT32::get_value_from_sector(this->BOOTSECTOR, 0x11, 2) * 32) / 512;
    this->sector_per_FAT = FAT32::get_value_from_sector(this->BOOTSECTOR, 0x24, 4);
    this->first_DATA_sector = this->sector_in_bootsector + this->num_of_FAT * this->sector_per_FAT;
    this->first_RDET_sector = this->sector_in_bootsector + this->num_of_FAT * this->sector_per_FAT + this->sector_in_RDET;

    FAT32::print_sector(this->BOOTSECTOR);
}

FAT32::~FAT32()
{
    delete[] BOOTSECTOR;
    delete[] FAT;
    
   
}

unsigned int FAT32::get_value_from_sector(BYTE* sector, int offset, int byte)
{
    unsigned int k = 0;
    memcpy(&k, sector + offset, byte);
    return k;
}


//hàm đọc dữ liệu sector trả về 1 nếu đọc thành công, 0 nếu đọc thất bại
int FAT32::readSector(int readPoint, BYTE*& sector, int size)
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(this->drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        cout << "CreateFile : " << GetLastError() << endl;
        cout << endl;
        return 0;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, size, &bytesRead, NULL))
    {
        cout << "Read_File_Content : " << GetLastError() << endl;
        return 0;
    }
    else
    {
        return 1;
    }
}


void FAT32::print_sector(BYTE* sector)
{
    int count = 0;
    int num = 0;

    cout << "         0  1  2  3  4  5  6  7    8  9  A  B  C  D  E  F" << endl;

    cout << "0x0" << num << "0  ";
    bool flag = 0;
    for (int i = 0; i < 512; i++)
    {
        count++;
        if (i % 8 == 0)
            cout << "  ";

        //in phần tử trong sector theo định dạng hexa: 
        printf("%02X ", sector[i]);

        if (i == 255)
        {
            flag = 1;
            num = 0;
        }

        if (i == 511) break;
        if (count == 16)
        {
            int index = i;

            cout << endl;

            if (flag == 0)
            {
                num++;
                if (num < 10)
                    cout << "0x0" << num << "0  ";
                else
                {
                    char hex = char(num - 10 + 'A');
                    cout << "0x0" << hex << "0  ";
                }

            }
            else
            {
                if (num < 10)
                    cout << "0x1" << num << "0  ";
                else
                {
                    char hex = char(num - 10 + 'A');
                    cout << "0x1" << hex << "0  ";
                }
                num++;
            }

            count = 0;
        }
    }
    cout << endl;
}

string FAT32::getString(BYTE* sector, int offset, int byte)
{
    char* name = new char[byte];
    memcpy(name, sector + offset, byte);
    string result = "";
    for (int i = 0; i < byte; i++)
        result += name[i];
    return result;
}


void FAT32::showInfo(string name, string status, unsigned int size, unsigned int firstCluster, unsigned int lastCluster)
{
    cout << "\n----- FILE / FOLDER -----\n";
    cout << "Name: " << name << endl;
    cout << "Status: " << status << endl;
    cout << "Size: " << size << endl;
    cout << "Used cluster: ";
    for (int i = firstCluster; i <= lastCluster; i++)
        cout << i << " ";
    cout << endl;
}


void FAT32::print_RDET()
{
    cout << "\n========== ROOT DIRECTORY ENTRY TABLE ==========\n";
    readRDET(this->first_DATA_sector, 0);
}

void FAT32::print_bootsector()
{
    cout << "\n========== BOOTSECTOR ==========\n";
    cout << "byte/sector: " << byte_per_sector << endl;
    cout << "sector/cluster: " << sector_per_cluster << endl;
    cout << "Bootsector's sector: " << sector_in_bootsector << endl;
    cout << "Number of FAT: " << num_of_FAT << endl;
    cout << "sector/FAT: " << sector_per_FAT << endl;
    cout << "first RDET sector: " << first_RDET_sector << endl;
    cout << "first data sector: " << first_DATA_sector << endl;
}

void FAT32::readRDET(int sector, int depth)
{

    //Những thông số cần thiết
    unsigned int size;
    unsigned int firstCluster, lastCluster;
    string status;
    string name;
    string file_type;
    BYTE* rdet = new BYTE[512];
    Object object_handle;
    //vì 2 entry đầu RDET là . và .. nên ta bỏ qua 2 entry này.
    int index = 2 * 32; 
    //mỗi sector = 512B nên để đến được sector cần đọc thì bắt đầu từ sector*512
    this->readSector(sector * 512, rdet, 512);
    while (true)
    {
        //Đã đọc tới cuối sector
        if (index == 512)
        {
            sector++;
            index = 0;
            this->readSector(sector * 512, rdet, 512);
        }

        //Kiểm tra xem bảng RDET còn được sử dụng không
        if (FAT32::get_value_from_sector(rdet, index + 0x0B,1) == 0x00)
            break;

        //kiểm tra entry phụ
        if (FAT32::get_value_from_sector(rdet, index + 0x0B, 1) == 0x0F);
        //kiểm tra entry chính (tập tin)
        else if (FAT32::get_value_from_sector(rdet, index + 0x0B, 1) == 0x20)
        {
          
            name = FAT32::getString(rdet, index, 8);
            name += ".";
            file_type = FAT32::getString(rdet, index + 8, 3);
            name += file_type;
            size = FAT32::get_value_from_sector(rdet, index + 0x1C, 4);
            status = "FILE";
            firstCluster = FAT32::get_value_from_sector(rdet, index + 0x1A, 2);
            lastCluster = firstCluster;
            int tmpIndex = firstCluster * 2+1;
            
            this->readSector(sector_in_bootsector*512, FAT, 512);
            while (FAT32::get_value_from_sector(FAT, tmpIndex, 2) != 65535)
            {
                lastCluster++;
                tmpIndex += 2;
            }
            list.push_back(new Object(name, depth));
            
            FAT32::showInfo(name, status, size, firstCluster, lastCluster);
            this->get_used_sector(firstCluster, lastCluster, first_DATA_sector, sector_per_cluster);

            //nếu là file.txt thì đọc nội dung file, không thì bỏ qua xử lý
            if (file_type == "TXT")
            {
                //đọc content file từ cluster bắt đầu của entry đó
                int content_sector = first_DATA_sector + (firstCluster - 2) * sector_per_cluster;
                print_file_content(content_sector);
            }
            else
                cout << "\nThis is not a text file. Please use compatible software to read file\n";
           
        }
        //kiểm tra entry chính (thư mục)
        else if (FAT32::get_value_from_sector(rdet, index + 0x0B, 1) == 0x10)
        {
            size = 0;
            name = FAT32::getString(rdet, index, 11);
            status = "DIRECTORY";
            firstCluster = FAT32::get_value_from_sector(rdet, index + 0x1A, 2);
            lastCluster = firstCluster;
            int tmpIndex = firstCluster * 2 + 1;
           
            this->readSector(sector_in_bootsector*512, FAT, 512);
            while (FAT32::get_value_from_sector(FAT, tmpIndex, 2) != 65535)
            {
                lastCluster++;
                tmpIndex += 2;
            }
            list.push_back(new Object(name, depth));
            FAT32::showInfo(name, status, size, firstCluster, lastCluster);
            this->get_used_sector(firstCluster, lastCluster, first_DATA_sector, sector_per_cluster);
            //nếu là tập tin, gọi đệ quy hàm readRDET để đọc thông tin trong tập tin đó
            this->readRDET(sector_in_bootsector + num_of_FAT*sector_per_FAT + (firstCluster-2)*sector_per_cluster, depth + 1);
        }
        index += 32;
    }
}

void FAT32::print_directory_tree()
{

    if (list.size() == 0)
        cout << "Empty directory tree\n";

    cout << "\n========== DIRECTORY TREE ==========\n";
    for (int i = 0; i < list.size(); i++)
    {
        int temp = list[i]->getDepth();
        while (temp--)
            cout << "\t";
        cout << list[i]->getName() << endl;
    }
}

void FAT32::read_FAT(BYTE* FAT, int startPoint)
{
    this->readSector(startPoint, FAT, 512);
}


void FAT32::get_used_sector(int firstCluster, int lastCluster, int firstDataSector, int sectorPerCluster)
{
    int res = 0;
    cout << "\nSector: ";
    for (int i = firstCluster; i <= lastCluster; i++)
    {
        res = firstDataSector + (i - 2) * sectorPerCluster;
        for (int j = 0; j < sectorPerCluster; j++)
            cout << res++ << " ";
    }
    cout << endl;
}

string FAT32::read_text_file(int sector)
{
    BYTE* data = new BYTE[512];
    FAT32::readSector(sector * 512, data, 512);
    //this->print_sector(data);
    int index = 0;
    string res = "";
    int check = FAT32::get_value_from_sector(data, index, 1);
    while (check != 0)
    {
        res += FAT32::getString(data, index, 1);
        index++;
        check = FAT32::get_value_from_sector(data, index, 1);
    }
    return res;
}

void FAT32::print_file_content(int sector)
{
    string content = read_text_file(sector);
    if (content != "")
    {
        cout << "\n---------- FILE'S CONTENT ----------\n";
        cout << read_text_file(sector) << endl;
    }
    else
        cout << "\nEmpty text file\n";
}
    
    
#include "FAT32.h"




int main()
{
	wstring driveName;
	cout << "Enter volume's name: ";
	//wcin đọc giá trị đầu vào dưới dạng wide character
	wcin >> driveName;
	//chuyển về định dạng đúng của ổ đĩa (vd ổ C có định dạng \\.\C:)
	driveName = L"\\\\.\\" + driveName + L":";
	LPCWSTR drive = driveName.c_str();

	FAT32 volume(drive);
	
	volume.print_bootsector();
	//volume.print_RDET();
	volume.print_directory_tree();

	


	return 0;
}
#if 0
#include <DirManager.h>
#include <Row.h>
#include <Table.h>
#include <CSVReader.h>

int main()
{
	
	auto data = pen::CSVReader::GetData("C:\\Users\\Ghevond\\Desktop\\AstroDataManipulations\\DataExtraction\\fbs1309_dataset.csv");

	pen::Table* table = new pen::Table{ data };
	

	

	delete table;
	return 0;
}
#endif
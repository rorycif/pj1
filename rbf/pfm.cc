#include "pfm.h"

PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager()
{
}


PagedFileManager::~PagedFileManager()
{
}


RC PagedFileManager::createFile(const string &fileName)
{
	it = pfms.find(fileName);
	if (it == pfms.end()){              //file not found

	   pfms[fileName] = fopen(fileName.c_str(), "w");                //creates blank file
	   fclose(pfms[fileName]);                    //closes file after create
	   return 0;
	}
	else{                               //file already exist*/
	   return -1;
	}
}


RC PagedFileManager::destroyFile(const string &fileName)
{
    it = pfms.find(fileName);
    if (it == pfms.end()){
        return -1;                      //file not found
    }
    else{
        remove(fileName.c_str());       //delete file
        pfms.erase(fileName);           //remove from map
        return 0;
    }
}


RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
    it = pfms.find(fileName);
    if (it == pfms.end()){
        return -1;                      //file does not exist
    }
    else{
        if (!fileHandle.isOpen){    	//nothing is open
        	pfms[fileName] = fopen(fileName.c_str(), "r+");
            fileHandle.isOpen = true;
            fileHandle.targetName = fileName;
            fileHandle.pfmPointer = this;
            return 0;
        } else {       					//a file is currently opened
            return -1;
        }
    }
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
    it = pfms.find(fileHandle.targetName);
    if (it == pfms.end()){
        return -1;                      			//file does not exist
    }

    fflush(pfms[fileHandle.targetName]);            //flushes everything changed to file

    fclose(pfms[fileHandle.targetName]);
    fileHandle.targetName = "";                     //empty refrence
    fileHandle.isOpen = false;                      //handle has closed file
    return 0;
}


FileHandle::FileHandle()
{
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
}


FileHandle::~FileHandle()
{
}


RC FileHandle::readPage(PageNum pageNum, void *data)
{
	if (appendPageCounter < pageNum){
		return -1;					//the page must not exist
	}
	//TODO find file
	//find page possibly with fseek()
	//read page to buffer with fread()

    readPageCounter++;                              //increments counter
    return 0;
}


RC FileHandle::writePage(PageNum pageNum, const void *data)
{
	if (appendPageCounter < pageNum){
		return -1;					//page must not exist
	}
	//TODO get to page w/ fseek()
	//write to page using fwrite()
    writePageCounter++;                             //increments counter
    return 0;
}


RC FileHandle::appendPage(const void *data)
{
    Page newPage;
    //newPage.data = data;
    newPage.freeSize = PAGE_SIZE - sizeof(&data);
    pageVector.push_back(newPage);

    appendPageCounter++;                            //increments counter
    return -1;
}


unsigned FileHandle::getNumberOfPages()
{
    return appendPageCounter;
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
    return -1;
}

Page::Page()
{
    freeSize = PAGE_SIZE;
}

Page::~Page()
{

}

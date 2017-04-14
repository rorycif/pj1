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
        pfms[fileName] = 0;             //saves file to map
        ofstream myfile;                //creates blank file
        myfile.open(fileName.c_str());
        myfile.close();
        return 0;
    }
    else{                               //file already exist
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
        if (!fileHandle.isOpen){    //nothing is open
            fileHandle.isOpen = true;
            fileHandle.targetName = fileName;
            return 0;
        }
        else if (fileHandle.isOpen && fileHandle.targetName != fileName){       //opened but new file to handle
            //fileHandle.targetName = fileName;
            return -1;
        }
        else{                           //opened already and handling it
            return 0;
        }
    }
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
 //   it = pfms.find(fileHandle.targetName);
 //   if (it == pfms.end()){
 //       return -1;                      //file does not exist
 //   }
    //todo flush files to disk
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
    readPageCounter++;                              //increments counter
    return -1;
}


RC FileHandle::writePage(PageNum pageNum, const void *data)
{
    writePageCounter++;                             //increments counter
    return -1;
}


RC FileHandle::appendPage(const void *data)
{
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

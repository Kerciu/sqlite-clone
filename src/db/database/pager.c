#include "database.h"

void* getPage(Pager* pager, uint32_t pageNum) {
    if (pageNum > TABLE_MAX_PAGES) {
        fprintf(stderr, "Unable to fetch page number in bounds (%d > %d)\n", pageNum, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[pageNum] == NULL) {        // Cache miss
        // Allocate memory and load from file
        void* page = malloc(PAGE_SIZE);
        uint32_t numOfPages = pager->fileLength / PAGE_SIZE;

        // Save partial page at the end of file
        if (pager->fileLength % PAGE_SIZE) {
            ++numOfPages;
        }

        if (pageNum <= numOfPages) {
            lseek(pager->fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);
            ssize_t bytesRead = read(pager->fileDescriptor, page, PAGE_SIZE);

            if (bytesRead == -1) {
                fprintf(stderr, "Failed to read file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[pageNum] = page;

        if (pageNum >= pager->numPages) {
            pager->numPages = pageNum + 1;
        }
    }

    return pager->pages[pageNum];
}

Pager* openPager(const char* fileHandle) {
    int fileDescript = open(fileHandle, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fileDescript == -1) {
        fprintf(stderr, "Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t fileLen = lseek(fileDescript, 0, SEEK_END);

    Pager* pager = (Pager*)malloc(sizeof(Pager));
    pager->fileDescriptor = fileDescript;
    pager->fileLength = fileLen;
    pager->numPages = (fileLen / PAGE_SIZE);

    if (fileLen % PAGE_SIZE != 0) {
        fprintf(stderr, "Data base file is not a whole number of pages. Corrunt file\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        pager->pages[i] = NULL;
    }

    return pager;
}

void pagerFlush(Pager* pager, uint32_t pageNum) {
    if (pager->pages[pageNum] == NULL) {
        fprintf(stderr, "Error: trying to flush NULL page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        fprintf(stderr, "Error occured while seeking %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytesWritten = write(pager->fileDescriptor, pager->pages[pageNum], PAGE_SIZE);
    if (bytesWritten == -1) {
        fprintf(stderr, "Error occured while writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

uint32_t getUnusedPageNum(Pager* pager) {
    return pager->numPages;
}

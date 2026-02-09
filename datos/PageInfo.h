#pragma once

#include <vector>
#include <cstddef>

namespace pos {

/**
 * @brief Pagination utility for displaying large datasets
 */
class PageInfo {
public:
    PageInfo(size_t pageSize = 50) : pageSize(pageSize) {}

    // Set total items (calculated from DB size)
    void setTotalItems(size_t total) {
        totalItems = total;
        totalPages = (total + pageSize - 1) / pageSize;  // ceil division
    }

    // Navigate to page
    bool goToPage(size_t page) {
        if (page < 1 || page > totalPages) return false;
        currentPage = page;
        return true;
    }

    // Get offsets for current page
    size_t getStartOffset() const {
        return (currentPage - 1) * pageSize;
    }

    size_t getEndOffset() const {
        return std::min(currentPage * pageSize, totalItems);
    }

    size_t getPageSize() const { return pageSize; }
    size_t getCurrentPage() const { return currentPage; }
    size_t getTotalPages() const { return totalPages; }
    size_t getTotalItems() const { return totalItems; }

    bool hasNextPage() const { return currentPage < totalPages; }
    bool hasPreviousPage() const { return currentPage > 1; }

    bool nextPage() { return goToPage(currentPage + 1); }
    bool previousPage() { return goToPage(currentPage > 1 ? currentPage - 1 : 1); }

private:
    size_t pageSize = 50;
    size_t currentPage = 1;
    size_t totalPages = 0;
    size_t totalItems = 0;
};

} // namespace pos

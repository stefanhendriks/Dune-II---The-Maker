#pragma once

#include "utils/cMatrix.h"
#include "utils/cPoint.h"

#include <array>
#include <variant>
#include <vector>

class cEditorUndoRedoHistory {
public:
    void clear();

    void beginRecordGroup();
    void recordTileChange(int col, int row, int oldTileID, int newTileID);
    void recordStartCellChange(int playerID, cPoint oldPos, cPoint newPos);

    bool applyUndo(Matrix<int> &mapData, std::array<cPoint, 5> &startCells);
    bool applyRedo(Matrix<int> &mapData, std::array<cPoint, 5> &startCells);

private:
    struct Marker {};
    struct TileChange { int col, row, oldTileID, newTileID; };
    struct StartCellChange { int playerID; cPoint oldPos, newPos; };
    using EditorChange = std::variant<Marker, TileChange, StartCellChange>;

    std::vector<EditorChange> m_undoStack;
    std::vector<EditorChange> m_redoStack;
};

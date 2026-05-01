#include "gamestates/cEditorState/cEditorUndoRedoHistory.h"

void cEditorUndoRedoHistory::clear()
{
    m_undoStack.clear();
    m_redoStack.clear();
}

void cEditorUndoRedoHistory::beginRecordGroup()
{
    m_undoStack.push_back(Marker{});
    m_redoStack.clear();
}

void cEditorUndoRedoHistory::recordTileChange(int col, int row, int oldTileID, int newTileID)
{
    m_undoStack.push_back(TileChange{col, row, oldTileID, newTileID});
}

void cEditorUndoRedoHistory::recordStartCellChange(int playerID, cPoint oldPos, cPoint newPos)
{
    m_undoStack.push_back(StartCellChange{playerID, oldPos, newPos});
}

bool cEditorUndoRedoHistory::applyUndo(Matrix<int> &mapData, std::array<cPoint, 5> &startCells)
{
    if (m_undoStack.empty()) return false;

    std::vector<EditorChange> group;
    while (!m_undoStack.empty() && !std::holds_alternative<Marker>(m_undoStack.back())) {
        group.push_back(m_undoStack.back());
        m_undoStack.pop_back();
    }
    if (!m_undoStack.empty()) {
        m_undoStack.pop_back();
    }

    if (group.empty()) return false;

    m_redoStack.push_back(Marker{});
    for (auto &change : group) {
        if (auto *tc = std::get_if<TileChange>(&change)) {
            mapData[tc->row][tc->col] = tc->oldTileID;
            m_redoStack.push_back(TileChange{tc->col, tc->row, tc->oldTileID, tc->newTileID});
        } else if (auto *sc = std::get_if<StartCellChange>(&change)) {
            startCells[sc->playerID] = sc->oldPos;
            m_redoStack.push_back(StartCellChange{sc->playerID, sc->oldPos, sc->newPos});
        }
    }

    return true;
}

bool cEditorUndoRedoHistory::applyRedo(Matrix<int> &mapData, std::array<cPoint, 5> &startCells)
{
    if (m_redoStack.empty()) return false;

    std::vector<EditorChange> group;
    while (!m_redoStack.empty() && !std::holds_alternative<Marker>(m_redoStack.back())) {
        group.push_back(m_redoStack.back());
        m_redoStack.pop_back();
    }
    if (!m_redoStack.empty()) {
        m_redoStack.pop_back();
    }

    if (group.empty()) return false;

    m_undoStack.push_back(Marker{});
    for (auto &change : group) {
        if (auto *tc = std::get_if<TileChange>(&change)) {
            mapData[tc->row][tc->col] = tc->newTileID;
            m_undoStack.push_back(TileChange{tc->col, tc->row, tc->oldTileID, tc->newTileID});
        } else if (auto *sc = std::get_if<StartCellChange>(&change)) {
            startCells[sc->playerID] = sc->newPos;
            m_undoStack.push_back(StartCellChange{sc->playerID, sc->oldPos, sc->newPos});
        }
    }

    return true;
}

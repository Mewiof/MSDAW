#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Undo/UndoableAction.h"

// two-stack undo/redo history. Callers apply their change to the model as usual,
// then Push() an action describing how to invert/re-apply it. Push() clears the
// redo stack. A depth cap drops the oldest actions once exceeded
//
// transactions group several pushes into a single undo step. BeginTransaction()
// starts collecting; every Push() during the transaction is buffered; the buffer
// is committed as one CompositeAction on EndTransaction(). Transactions do not
// nest in practice, but a depth counter keeps a stray Begin/End balanced
class UndoManager {
public:
	UndoManager() = default;

	// take ownership of an already-applied action
	void Push(std::unique_ptr<UndoableAction> action);

	bool CanUndo() const;
	bool CanRedo() const;

	void Undo();
	void Redo();

	void Clear();

	// menu labels ("Undo <name>"); return nullptr when the stack is empty
	const char* PeekUndoName() const;
	const char* PeekRedoName() const;

	// group multiple pushes into one undo step
	void BeginTransaction(const char* name);
	void EndTransaction();

	// ---- history inspection (for the History window) ----
	// full chronological list of edit labels: applied actions first (oldest to
	// newest), then redoable actions (next-to-redo first). Excludes the base state
	std::vector<std::string> GetHistory() const;
	// number of applied (undoable) actions; also the index of the current state
	// in a list whose row 0 is the base state
	size_t GetAppliedCount() const { return mUndoStack.size(); }
private:
	std::vector<std::unique_ptr<UndoableAction>> mUndoStack;
	std::vector<std::unique_ptr<UndoableAction>> mRedoStack;

	static constexpr size_t kMaxDepth = 200;

	// transaction state
	int mTransactionDepth = 0;
	const char* mTransactionName = "Edit";
	std::vector<std::unique_ptr<UndoableAction>> mTransactionBuffer;
};

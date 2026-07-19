#include "PrecompHeader.h"
#include "Undo/UndoManager.h"

void UndoManager::Push(std::unique_ptr<UndoableAction> action) {
	if (!action)
		return;

	// during a transaction, buffer instead of committing immediately
	if (mTransactionDepth > 0) {
		mTransactionBuffer.push_back(std::move(action));
		return;
	}

	mRedoStack.clear();
	mUndoStack.push_back(std::move(action));

	// trim oldest entries past the depth cap
	if (mUndoStack.size() > kMaxDepth)
		mUndoStack.erase(mUndoStack.begin(), mUndoStack.begin() + (mUndoStack.size() - kMaxDepth));
}

bool UndoManager::CanUndo() const {
	return !mUndoStack.empty();
}

bool UndoManager::CanRedo() const {
	return !mRedoStack.empty();
}

void UndoManager::Undo() {
	if (mUndoStack.empty())
		return;
	std::unique_ptr<UndoableAction> action = std::move(mUndoStack.back());
	mUndoStack.pop_back();
	action->Undo();
	mRedoStack.push_back(std::move(action));
}

void UndoManager::Redo() {
	if (mRedoStack.empty())
		return;
	std::unique_ptr<UndoableAction> action = std::move(mRedoStack.back());
	mRedoStack.pop_back();
	action->Redo();
	mUndoStack.push_back(std::move(action));
}

void UndoManager::Clear() {
	mUndoStack.clear();
	mRedoStack.clear();
	mTransactionBuffer.clear();
	mTransactionDepth = 0;
}

const char* UndoManager::PeekUndoName() const {
	return mUndoStack.empty() ? nullptr : mUndoStack.back()->Name();
}

const char* UndoManager::PeekRedoName() const {
	return mRedoStack.empty() ? nullptr : mRedoStack.back()->Name();
}

std::vector<std::string> UndoManager::GetHistory() const {
	std::vector<std::string> labels;
	labels.reserve(mUndoStack.size() + mRedoStack.size());
	for (const auto& a : mUndoStack)
		labels.push_back(a->Name());
	// redo stack back() is the next action to redo (chronologically after current)
	for (auto it = mRedoStack.rbegin(); it != mRedoStack.rend(); ++it)
		labels.push_back((*it)->Name());
	return labels;
}

void UndoManager::BeginTransaction(const char* name) {
	if (mTransactionDepth == 0) {
		mTransactionName = (name && name[0]) ? name : "Edit";
		mTransactionBuffer.clear();
	}
	mTransactionDepth++;
}

void UndoManager::EndTransaction() {
	if (mTransactionDepth == 0)
		return;
	mTransactionDepth--;
	if (mTransactionDepth > 0)
		return; // still inside an outer transaction

	if (mTransactionBuffer.empty())
		return;

	if (mTransactionBuffer.size() == 1) {
		// a single action needs no wrapper
		Push(std::move(mTransactionBuffer.front()));
		mTransactionBuffer.clear();
		return;
	}

	auto composite = std::make_unique<CompositeAction>(mTransactionName);
	for (auto& a : mTransactionBuffer)
		composite->Add(std::move(a));
	mTransactionBuffer.clear();
	Push(std::move(composite));
}

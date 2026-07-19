#pragma once
#include <vector>
#include <memory>

// base interface for a single reversible edit. Concrete actions live in
// Undo/Actions.h. An action is created AFTER the change has already been applied
// to the model (the UI mutates as it did before undo existed); Undo() reverts it
// and Redo() re-applies it
class UndoableAction {
public:
	virtual ~UndoableAction() = default;

	virtual void Undo() = 0;
	virtual void Redo() = 0;

	// short human-readable label shown in the Edit menu (e.g. "Parameter change")
	virtual const char* Name() const { return "Edit"; }
};

// bundles several actions into one undo step. Undo reverses them in reverse
// order; Redo replays them in original order. Used by transactions and by
// compound edits (e.g. a cross-track device move = remove + insert)
class CompositeAction : public UndoableAction {
public:
	explicit CompositeAction(const char* name) : mName(name) {}

	void Add(std::unique_ptr<UndoableAction> action) {
		if (action)
			mActions.push_back(std::move(action));
	}

	bool Empty() const { return mActions.empty(); }
	size_t Size() const { return mActions.size(); }

	void Undo() override {
		for (auto it = mActions.rbegin(); it != mActions.rend(); ++it)
			(*it)->Undo();
	}
	void Redo() override {
		for (auto& a : mActions)
			a->Redo();
	}
	const char* Name() const override { return mName; }
private:
	const char* mName;
	std::vector<std::unique_ptr<UndoableAction>> mActions;
};

#include "PrecompHeader.h"
#include "TrackLayout.h"
#include "EditorContext.h"
#include "Project.h"

namespace TrackLayout {

	std::vector<Row> Build(EditorContext& context) {
		std::vector<Row> rows;
		Project* project = context.GetProject();
		if (!project)
			return rows;

		auto& tracks = project->GetTracks();
		rows.resize(tracks.size());

		float fullHeight = context.layout.trackRowHeight + context.layout.trackGap;
		float collapsedHeight = context.layout.trackCollapsedHeight;

		float y = 0.0f;
		for (size_t i = 0; i < tracks.size(); ++i) {
			auto& t = tracks[i];

			// a track is hidden when any ancestor group is folded
			bool hidden = false;
			for (auto p = t->GetParent(); p; p = p->GetParent()) {
				if (p->mIsCollapsed) {
					hidden = true;
					break;
				}
			}

			rows[i].top = y;
			if (hidden) {
				rows[i].visible = false;
				rows[i].height = 0.0f;
			} else {
				rows[i].visible = true;
				// only non-group tracks shrink; a collapsed group keeps its header
				// height and merely folds its children away (handled above)
				rows[i].height = (t->mIsCollapsed && !t->IsGroup()) ? collapsedHeight : fullHeight;
				y += rows[i].height;
			}
		}
		return rows;
	}

	float TotalHeight(const std::vector<Row>& rows) {
		float h = 0.0f;
		for (const auto& r : rows)
			h += r.height;
		return h;
	}

	int RowAtY(const std::vector<Row>& rows, float yOffset) {
		int last = -1;
		for (int i = 0; i < (int)rows.size(); ++i) {
			if (!rows[i].visible)
				continue;
			if (yOffset < rows[i].top + rows[i].height)
				return i;
			last = i;
		}
		return last;
	}

} // namespace TrackLayout

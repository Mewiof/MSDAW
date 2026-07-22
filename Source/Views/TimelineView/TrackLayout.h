#pragma once
#include <vector>

struct EditorContext;

// shared, single-source-of-truth vertical layout for the arrangement's tracks.
// both the track-list column and the timeline lanes derive each track's Y band
// from this so collapsing/folding stays in sync between the two views
namespace TrackLayout {
	struct Row {
		float top = 0.0f;	  // y offset from the track area's top for this row
		float height = 0.0f;  // this row's full height (0 for hidden rows)
		bool visible = false; // false when an ancestor group is folded (collapsed)
	};

	// build one Row per project track, honoring per-track collapse:
	// - a collapsed non-group track shrinks to layout.trackCollapsedHeight
	// - a collapsed group folds: all of its descendants become hidden (height 0)
	// the returned vector is indexed 1:1 with project->GetTracks()
	std::vector<Row> Build(EditorContext& context);

	// stacked height of all visible rows
	float TotalHeight(const std::vector<Row>& rows);

	// index of the visible row whose band contains yOffset (measured from the track
	// area's top). clamps to the first/last visible row when out of range, or -1 if
	// there are no visible rows
	int RowAtY(const std::vector<Row>& rows, float yOffset);
} // namespace TrackLayout

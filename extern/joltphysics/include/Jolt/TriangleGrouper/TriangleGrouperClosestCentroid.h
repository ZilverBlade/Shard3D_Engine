// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include <TriangleGrouper/TriangleGrouper.h>

namespace JPH {

/// A class that groups triangles in batches of N.
/// Starts with centroid with lowest X coordinate and finds N closest centroids, this repeats until all groups have been found.
/// Time complexity: O(N^2)
class TriangleGrouperClosestCentroid : public TriangleGrouper
{
public:
	// See: TriangleGrouper::Group
	virtual void			Group(const VertexList &inVertices, const IndexedTriangleList &inTriangles, int inGroupSize, vector<uint> &outGroupedTriangleIndices) override;
};

} // JPH
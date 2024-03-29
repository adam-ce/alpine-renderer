/*****************************************************************************
 * Alpine Terrain Builder
 * Copyright (C) 2022 alpinemaps.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#pragma once

#include <QObject>

#include "alpine_renderer/TileScheduler.h"
#include "alpine_renderer/utils/QuadTree.h"

class BasicTreeTileScheduler : public TileScheduler
{
  enum class TileStatus {
    Uninitialised,
    Unavailable,
    InTransit,
    WaitingForSiblings,
    OnGpu
  };
  struct NodeData {
    srs::TileId id = {};
    TileStatus status = TileStatus::Uninitialised;
  };
  using Node = QuadTreeNode<NodeData>;

  std::unique_ptr<Node> m_root_node;
  Tile2DataMap m_received_ortho_tiles;
  Tile2DataMap m_received_height_tiles;
  TileSet m_gpu_tiles_to_be_expired;

  bool m_enabled = true;

public:
  BasicTreeTileScheduler();

  size_t numberOfTilesInTransit() const override;
  size_t numberOfWaitingHeightTiles() const override;
  size_t numberOfWaitingOrthoTiles() const override;
  TileSet gpuTiles() const override;
  bool enabled() const override;
  void setEnabled(bool newEnabled) override;

public slots:
  void updateCamera(const Camera& camera) override;
  void receiveOrthoTile(srs::TileId tile_id, std::shared_ptr<QByteArray> data) override;
  void receiveHeightTile(srs::TileId tile_id, std::shared_ptr<QByteArray> data) override;
  void notifyAboutUnavailableOrthoTile(srs::TileId tile_id) override;
  void notifyAboutUnavailableHeightTile(srs::TileId tile_id) override;

//signals:
//  void tileRequested(const srs::TileId& tile_id);
//  void tileReady(const std::shared_ptr<Tile>& tile);
//  void tileExpired(const srs::TileId& tile_id);
//  void cancelTileRequest(const srs::TileId& tile_id);

private:
  void checkConsistency() const;
  void checkLoadedTile(const srs::TileId& tile_id);
  void markTileUnavailable(const srs::TileId& tile_id);
};


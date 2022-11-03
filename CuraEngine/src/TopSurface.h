//Copyright (c) 2018 Ultimaker B.V.
//CuraEngine is released under the terms of the AGPLv3 or higher.

#ifndef TOPSURFACE_H
#define TOPSURFACE_H

#include "utils/polygon.h" //For the polygon areas.

namespace cura
{

class GCodePathConfig;
class LayerPlan;
class SliceMeshStorage;

class TopSurface
{
public:
    /*!
     * Create an empty top surface area.
     */
    TopSurface();

    /*!
     * \brief Sets the areas for this TopSurface to process from the given
     *        mesh and layer number.
     *
     * The surface will be generated by subtracting the layer above from the
     * current layer. Anything that is leftover is then part of the top surface
     * (since there is nothing above it).
     *
     * \param mesh The mesh to generate the top surface area for.
     * \param layer_number The layer to generate the top surface area for.
     */
    void setAreasFromMeshAndLayerNumber(SliceMeshStorage& mesh, size_t layer_number);

    /*!
     * \brief Generate paths for ironing over the top surface.
     *
     * This generates an infill pattern over the top surface that is supposed to
     * strike the surface smooth by melting it with the hot nozzle and filling
     * crevices with a minute amount of material.
     *
     * \param mesh The settings base to get our ironing settings and skin angles
     * from.
     * \param line_config The configuration of the ironing lines to use. Note
     * that the flow might still get adjusted by the ironing settings.
     * \param[out] layer The output g-code layer to put the resulting lines in.
     */
    bool ironing(const SliceMeshStorage& mesh, const GCodePathConfig& line_config, LayerPlan& layer) const;

public:
    /*!
     * \brief The areas of top surface, for each layer.
     */
    Polygons areas;
};

}

#endif /* TOPSURFACE_H */

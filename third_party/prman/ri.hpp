#ifndef RI_HPP
#define RI_HPP /* RenderMan Interface, C++ binding */

#define RI_VERSION 4

/*
# ------------------------------------------------------------------------------
#
# Copyright (c) 1997-2012 Pixar Animation Studios. All rights reserved.
#
# The information in this file (the "Software") is provided for the
# exclusive use of the software licensees of Pixar.  Licensees have
# the right to incorporate the Software into other products for use
# by other authorized software licensees of Pixar, without fee.
# Except as expressly permitted herein, the Software may not be
# disclosed to third parties, copied or duplicated in any form, in
# whole or in part, without the prior written permission of
# Pixar Animation Studios.
#
# The copyright notices in the Software and this entire statement,
# including the above license grant, this restriction and the
# following disclaimer, must be included in all copies of the
# Software, in whole or in part, and all permitted derivative works of
# the Software, unless such copies or derivative works are solely
# in the form of machine-executable object code generated by a
# source language processor.
#
# PIXAR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
# ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL PIXAR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
# SOFTWARE.
#
# Pixar
# 1200 Park Ave
# Emeryville CA 94608
#
# ------------------------------------------------------------------------------
*/
#include "ri.h"

class Ri /* a pure-virtual interface */
{
public:
virtual ~Ri() {};


virtual RtArchiveHandle ArchiveBegin(RtToken name, ...)=0;
virtual RtArchiveHandle ArchiveBeginV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            ArchiveEnd()=0;
virtual void            ArchiveRecord(RtToken type, char* format, ...)=0;
virtual RtLightHandle   AreaLightSource(RtToken name, ...)=0;
virtual RtLightHandle   AreaLightSourceV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Atmosphere(RtToken name, ...)=0;
virtual void            AtmosphereV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Attribute(RtToken name, ...)=0;
virtual void            AttributeV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            AttributeBegin()=0;
virtual void            AttributeEnd()=0;
virtual void            Basis(RtBasis u, RtInt ustep, RtBasis v, RtInt vstep)=0;
virtual void            Begin(RtToken name)=0;
virtual void            Blobby(RtInt nleaf, RtInt ninst, RtInt* inst, 
                             RtInt nflt, RtFloat* flt, RtInt nstr, 
                             RtToken* str, ...)=0;
virtual void            BlobbyV(RtInt nleaf, RtInt ninst, 
                             RtInt* inst, RtInt nflt, RtFloat* flt, 
                             RtInt nstr, RtToken* str, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Bound(RtBound bound)=0;
virtual void            Bxdf(RtToken name, RtToken handle, ...)=0;
virtual void            BxdfV(RtToken name, RtToken handle, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Camera(RtToken name, ...)=0;
virtual void            CameraV(RtToken name, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Clipping(RtFloat nearplane, RtFloat farplane)=0;
virtual void            ClippingPlane(RtFloat Nx, RtFloat Ny, 
                             RtFloat Nz, RtFloat Px, RtFloat Py, 
                             RtFloat Pz)=0;
virtual void            Color(RtFloat* color)=0;
virtual void            ColorSamples(RtInt n, RtFloat* nRGB, RtFloat* RGBn)=0;
virtual void            ConcatTransform(RtMatrix m)=0;
virtual void            Cone(RtFloat height, RtFloat radius, RtFloat tmax, ...)=0;
virtual void            ConeV(RtFloat height, RtFloat radius, 
                             RtFloat tmax, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Context(RtContextHandle ctx)=0;
virtual void            CoordSysTransform(RtToken fromspace)=0;
virtual void            CoordinateSystem(char * name)=0;
virtual void            CropWindow(RtFloat left, RtFloat right, 
                             RtFloat bottom, RtFloat top)=0;
virtual void            Curves(RtToken type, RtInt ncurves, 
                             RtInt* nvertices, RtToken wrap, ...)=0;
virtual void            CurvesV(RtToken type, RtInt ncurves, 
                             RtInt* nvertices, RtToken wrap, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Cylinder(RtFloat radius, RtFloat zmin, 
                             RtFloat zmax, RtFloat tmax, ...)=0;
virtual void            CylinderV(RtFloat radius, RtFloat zmin, 
                             RtFloat zmax, RtFloat tmax, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual RtToken         Declare(char * name, char * decl)=0;
virtual void            Deformation(RtToken name, ...)=0;
virtual void            DeformationV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            DepthOfField(RtFloat fstop, RtFloat length, 
                             RtFloat distance)=0;
virtual void            Detail(RtBound bound)=0;
virtual void            DetailRange(RtFloat minvis, RtFloat lotrans, 
                             RtFloat hitrans, RtFloat maxvis)=0;
virtual void            Disk(RtFloat height, RtFloat radius, RtFloat tmax, ...)=0;
virtual void            DiskV(RtFloat height, RtFloat radius, 
                             RtFloat tmax, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Displacement(RtToken name, ...)=0;
virtual void            DisplacementV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Display(char * name, RtToken type, RtToken mode, ...)=0;
virtual void            DisplayV(char * name, RtToken type, 
                             RtToken mode, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            DisplayChannel(RtToken channel, ...)=0;
virtual void            DisplayChannelV(RtToken channel, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            EditAttributeBegin(RtToken name)=0;
virtual void            EditAttributeEnd()=0;
virtual void            EditBegin(RtToken name, ...)=0;
virtual void            EditBeginV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            EditEnd()=0;
virtual void            EditWorldBegin(RtToken name, ...)=0;
virtual void            EditWorldBeginV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            EditWorldEnd()=0;
virtual void            Else()=0;
virtual void            ElseIf(char * expr, ...)=0;
virtual void            ElseIfV(char * expr, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            End()=0;
virtual void            ErrorHandler(RtErrorHandler handler)=0;
virtual void            Exposure(RtFloat gain, RtFloat gamma)=0;
virtual void            Exterior(RtToken name, ...)=0;
virtual void            ExteriorV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Format(RtInt xres, RtInt yres, RtFloat pixel_ar)=0;
virtual void            FrameAspectRatio(RtFloat ar)=0;
virtual void            FrameBegin(RtInt frame)=0;
virtual void            FrameEnd()=0;
virtual void            GeneralPolygon(RtInt nloops, RtInt* nverts, ...)=0;
virtual void            GeneralPolygonV(RtInt nloops, RtInt* nverts, 
                             RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            GeometricApproximation(RtToken type, RtFloat value)=0;
virtual void            Geometry(RtToken name, ...)=0;
virtual void            GeometryV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual RtContextHandle GetContext()=0;
virtual void            Hider(RtToken name, ...)=0;
virtual void            HiderV(RtToken name, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            HierarchicalSubdivisionMesh(RtToken mask, 
                             RtInt nf, RtInt* nverts, RtInt* verts, 
                             RtInt nt, RtToken* tags, RtInt* nargs, 
                             RtInt* intargs, RtFloat* floatargs, 
                             RtToken* stringargs, ...)=0;
virtual void            HierarchicalSubdivisionMeshV(RtToken mask, 
                             RtInt nf, RtInt* nverts, RtInt* verts, 
                             RtInt nt, RtToken* tags, RtInt* nargs, 
                             RtInt* intargs, RtFloat* floatargs, 
                             RtToken* stringargs, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Hyperboloid(RtPoint point1, RtPoint point2, 
                             RtFloat tmax, ...)=0;
virtual void            HyperboloidV(RtPoint point1, RtPoint point2, 
                             RtFloat tmax, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Identity()=0;
virtual void            IfBegin(char * expr, ...)=0;
virtual void            IfBeginV(char * expr, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            IfEnd()=0;
virtual void            Illuminate(RtLightHandle light, RtBoolean onoff)=0;
virtual void            Imager(RtToken name, ...)=0;
virtual void            ImagerV(RtToken name, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Integrator(RtToken name, RtToken handle, ...)=0;
virtual void            IntegratorV(RtToken name, RtToken handle, 
                             RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            Interior(RtToken name, ...)=0;
virtual void            InteriorV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual RtLightHandle   LightSource(RtToken name, ...)=0;
virtual RtLightHandle   LightSourceV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            MakeBrickMap(RtInt nptcs, char ** ptcs, 
                             char * bkm, ...)=0;
virtual void            MakeBrickMapV(RtInt nptcs, char ** ptcs, 
                             char * bkm, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            MakeBump(char * pic, char * text, 
                             RtToken swrap, RtToken twrap, 
                             RtFilterFunc filt, RtFloat swidth, 
                             RtFloat twidth, ...)=0;
virtual void            MakeBumpV(char * pic, char * text, 
                             RtToken swrap, RtToken twrap, 
                             RtFilterFunc filt, RtFloat swidth, 
                             RtFloat twidth, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            MakeCubeFaceEnvironment(char * px, char * nx, 
                             char * py, char * ny, char * pz, char * nz, 
                             char * text, RtFloat fov, RtFilterFunc filt, 
                             RtFloat swidth, RtFloat twidth, ...)=0;
virtual void            MakeCubeFaceEnvironmentV(char * px, 
                             char * nx, char * py, char * ny, char * pz, 
                             char * nz, char * text, RtFloat fov, 
                             RtFilterFunc filt, RtFloat swidth, 
                             RtFloat twidth, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            MakeLatLongEnvironment(char * pic, 
                             char * text, RtFilterFunc filt, 
                             RtFloat swidth, RtFloat twidth, ...)=0;
virtual void            MakeLatLongEnvironmentV(char * pic, 
                             char * text, RtFilterFunc filt, 
                             RtFloat swidth, RtFloat twidth, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            MakeShadow(char * pic, char * text, ...)=0;
virtual void            MakeShadowV(char * pic, char * text, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            MakeTexture(char * pic, char * text, 
                             RtToken swrap, RtToken twrap, 
                             RtFilterFunc filt, RtFloat swidth, 
                             RtFloat twidth, ...)=0;
virtual void            MakeTextureV(char * pic, char * text, 
                             RtToken swrap, RtToken twrap, 
                             RtFilterFunc filt, RtFloat swidth, 
                             RtFloat twidth, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Matte(RtBoolean onoff)=0;
virtual void            MotionBegin(RtInt n, ...)=0;
virtual void            MotionBeginV(RtInt n, RtFloat vals[])=0;
virtual void            MotionEnd()=0;
virtual void            NuPatch(RtInt nu, RtInt uorder, 
                             RtFloat* uknot, RtFloat umin, RtFloat umax, 
                             RtInt nv, RtInt vorder, RtFloat* vknot, 
                             RtFloat vmin, RtFloat vmax, ...)=0;
virtual void            NuPatchV(RtInt nu, RtInt uorder, 
                             RtFloat* uknot, RtFloat umin, RtFloat umax, 
                             RtInt nv, RtInt vorder, RtFloat* vknot, 
                             RtFloat vmin, RtFloat vmax, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual RtObjectHandle  ObjectBegin()=0;
virtual RtObjectHandle  ObjectBeginV(RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            ObjectEnd()=0;
virtual void            ObjectInstance(RtObjectHandle h)=0;
virtual void            ObjectInstanceV(RtObjectHandle h, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Opacity(RtFloat* color)=0;
virtual void            Option(RtToken name, ...)=0;
virtual void            OptionV(RtToken name, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Orientation(RtToken orient)=0;
virtual void            Paraboloid(RtFloat radius, RtFloat zmin, 
                             RtFloat zmax, RtFloat tmax, ...)=0;
virtual void            ParaboloidV(RtFloat radius, RtFloat zmin, 
                             RtFloat zmax, RtFloat tmax, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Patch(RtToken type, ...)=0;
virtual void            PatchV(RtToken type, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            PatchMesh(RtToken type, RtInt nu, 
                             RtToken uwrap, RtInt nv, RtToken vwrap, ...)=0;
virtual void            PatchMeshV(RtToken type, RtInt nu, 
                             RtToken uwrap, RtInt nv, RtToken vwrap, 
                             RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            Pattern(RtToken name, RtToken handle, ...)=0;
virtual void            PatternV(RtToken name, RtToken handle, 
                             RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            Perspective(RtFloat fov)=0;
virtual void            PixelFilter(RtFilterFunc func, 
                             RtFloat xwidth, RtFloat ywidth)=0;
virtual void            PixelSampleImager(RtToken name, ...)=0;
virtual void            PixelSampleImagerV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            PixelSamples(RtFloat x, RtFloat y)=0;
virtual void            PixelVariance(RtFloat var)=0;
virtual void            Points(RtInt nverts, ...)=0;
virtual void            PointsV(RtInt nverts, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            PointsGeneralPolygons(RtInt npolys, 
                             RtInt* nloops, RtInt* nverts, RtInt* verts, 
                             ...)=0;
virtual void            PointsGeneralPolygonsV(RtInt npolys, 
                             RtInt* nloops, RtInt* nverts, RtInt* verts, 
                             RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            PointsPolygons(RtInt npolys, RtInt* nverts, 
                             RtInt* verts, ...)=0;
virtual void            PointsPolygonsV(RtInt npolys, RtInt* nverts, 
                             RtInt* verts, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Polygon(RtInt nverts, ...)=0;
virtual void            PolygonV(RtInt nverts, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Procedural(RtPointer data, RtBound bound, 
                             RtProcSubdivFunc sfunc, RtProcFreeFunc ffunc)=0;
virtual void            Procedural2(RtProc2SubdivFunc sfunc, 
                             RtProc2BoundFunc bfunc, ...)=0;
virtual void            Procedural2V(RtProc2SubdivFunc sfunc, 
                             RtProc2BoundFunc bfunc, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Projection(RtToken name, ...)=0;
virtual void            ProjectionV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Quantize(RtToken type, RtInt one, RtInt min, 
                             RtInt max, RtFloat dither)=0;
virtual void            ReadArchive(RtToken name, 
                             RtArchiveCallback callback, ...)=0;
virtual void            ReadArchiveV(RtToken name, 
                             RtArchiveCallback callback, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            RelativeDetail(RtFloat rel)=0;
virtual void            Resource(RtToken handle, RtToken type, ...)=0;
virtual void            ResourceV(RtToken handle, RtToken type, 
                             RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            ResourceBegin()=0;
virtual void            ResourceEnd()=0;
virtual void            ReverseOrientation()=0;
virtual void            Rotate(RtFloat angle, RtFloat dx, RtFloat dy, 
                             RtFloat dz)=0;
virtual void            Scale(RtFloat sx, RtFloat sy, RtFloat sz)=0;
virtual void            ScopedCoordinateSystem(char * name)=0;
virtual void            ScreenWindow(RtFloat left, RtFloat right, 
                             RtFloat bottom, RtFloat top)=0;
virtual void            Shader(RtToken name, RtToken handle, ...)=0;
virtual void            ShaderV(RtToken name, RtToken handle, 
                             RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            ShadingInterpolation(RtToken type)=0;
virtual void            ShadingRate(RtFloat size)=0;
virtual void            Shutter(RtFloat opentime, RtFloat closetime)=0;
virtual void            Sides(RtInt n)=0;
virtual void            Skew(RtFloat angle, RtFloat d1x, RtFloat d1y, 
                             RtFloat d1z, RtFloat d2x, RtFloat d2y, 
                             RtFloat d2z)=0;
virtual void            SolidBegin(char * op)=0;
virtual void            SolidEnd()=0;
virtual void            Sphere(RtFloat radius, RtFloat zmin, 
                             RtFloat zmax, RtFloat tmax, ...)=0;
virtual void            SphereV(RtFloat radius, RtFloat zmin, 
                             RtFloat zmax, RtFloat tmax, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            SubdivisionMesh(RtToken mask, RtInt nf, 
                             RtInt* nverts, RtInt* verts, RtInt nt, 
                             RtToken* tags, RtInt* nargs, RtInt* intargs, 
                             RtFloat* floatargs, ...)=0;
virtual void            SubdivisionMeshV(RtToken mask, RtInt nf, 
                             RtInt* nverts, RtInt* verts, RtInt nt, 
                             RtToken* tags, RtInt* nargs, RtInt* intargs, 
                             RtFloat* floatargs, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            Surface(RtToken name, ...)=0;
virtual void            SurfaceV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            System(char * name)=0;
virtual void            TextureCoordinates(RtFloat s1, RtFloat t1, 
                             RtFloat s2, RtFloat t2, RtFloat s3, 
                             RtFloat t3, RtFloat s4, RtFloat t4)=0;
virtual void            Torus(RtFloat majrad, RtFloat minrad, 
                             RtFloat phimin, RtFloat phimax, RtFloat tmax, 
                             ...)=0;
virtual void            TorusV(RtFloat majrad, RtFloat minrad, 
                             RtFloat phimin, RtFloat phimax, RtFloat tmax, 
                             RtInt n, RtToken nms[], RtPointer vals[])=0;
virtual void            Transform(RtMatrix m)=0;
virtual void            TransformBegin()=0;
virtual void            TransformEnd()=0;
virtual RtPoint *       TransformPoints(RtToken fromspace, 
                             RtToken tospace, RtInt n, RtPoint * points)=0;
virtual void            Translate(RtFloat dx, RtFloat dy, RtFloat dz)=0;
virtual void            TrimCurve(RtInt nloops, RtInt* ncurves, 
                             RtInt* order, RtFloat* knot, RtFloat* min, 
                             RtFloat* max, RtInt* n, RtFloat* u, 
                             RtFloat* v, RtFloat* w)=0;
virtual void            VArchiveRecord(RtToken type, char* format, va_list vap)=0;
virtual void            VPAtmosphere(RtToken name, ...)=0;
virtual void            VPAtmosphereV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            VPInterior(RtToken name, ...)=0;
virtual void            VPInteriorV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            VPSurface(RtToken name, ...)=0;
virtual void            VPSurfaceV(RtToken name, RtInt n, 
                             RtToken nms[], RtPointer vals[])=0;
virtual void            Volume(RtToken type, RtBound bound, 
                             RtInt* dimensions, ...)=0;
virtual void            VolumeV(RtToken type, RtBound bound, 
                             RtInt* dimensions, RtInt n, RtToken nms[], 
                             RtPointer vals[])=0;
virtual void            VolumePixelSamples(RtFloat x, RtFloat y)=0;
virtual void            WorldBegin()=0;
virtual void            WorldEnd()=0;
};


#endif /* RI_HPP */


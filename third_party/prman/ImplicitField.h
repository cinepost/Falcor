/* $Id: //depot/branches/rmanprod/rman-19.0/include/ImplicitField.h#4 $ $Date: 2014/12/10 $ */
/*
# ------------------------------------------------------------------------------
#
# Copyright (c) 2006-2014 Pixar Animation Studios. All rights reserved.
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
#ifndef IMPLICITFIELD_H
#define	IMPLICITFIELD_H

#include "ri.h"
#include <vector>

class ImplicitVertexValue {
private:
    /* inhibit copying */
    ImplicitVertexValue(const ImplicitVertexValue &);
    ImplicitVertexValue &operator=(const ImplicitVertexValue &);

public:
    ImplicitVertexValue() {}
    virtual ~ImplicitVertexValue() {}

    /// Stores in result the value of the named vertex variable
    /// associated with the call to CreateVertexValue that created
    /// this ImplicitVertexValue, evaluated at point p in object
    /// space at shutter open. When this function is called, the
    /// result parameter already has a legal value computed (generally
    /// the value bound in RIB to the geometry).
    virtual void GetVertexValue(RtFloat *result, const RtPoint p) = 0;

    /// Stores in result the filtered value of the named vertex variable
    /// associated with the call to CreateVertexValue that created
    /// this ImplicitVertexValue, evaluated over the region delineated
    /// by point p, dPdu, dPdv, and dPdw in object space at shutter
    /// open. When this function is called, the result parameter
    /// already has a legal value computed (generally the value bound
    /// in RIB to the geometry).
    virtual void GetVertexValueFiltered(RtFloat *result, const RtPoint p,
        const RtPoint dPdu, const RtPoint dPdv, const RtPoint dPdw) {
        GetVertexValue(result, p);
    }

    /// Stores in result the values of the named vertex variable
    /// associated with the call to CreateVertexValue that created
    /// this ImplicitVertexValue, evaluated at neval points p in
    /// object space at shutter open. When this function is called,
    /// the result parameter already has a legal value computed
    /// (generally the value bound in RIB to the geometry). The DSO
    /// must skip by resultstride when storing the results between
    /// each evaluation.
    virtual void GetVertexValueMultiple(int neval, RtFloat *result,
        int resultstride, const RtPoint *p) {
        for (int i = 0; i < neval; ++i) {
            GetVertexValue(result, *p++);
            result += resultstride;
        }
    }

    /// Stores in result the filtered values of the named vertex
    /// variable associated with the call to CreateVertexValue that
    /// created this ImplicitVertexValue, evaluated at neval regions
    /// delineated by points p, dPdu, dPdv, and dPdw in object space
    /// at shutter open. When this function is called, the result
    /// parameter already has a legal value computed (generally the
    /// value bound in RIB to the geometry). The DSO must skip by
    /// resultstride when storing the results between each evaluation.
    virtual void GetVertexValueMultipleFiltered(int neval, RtFloat *result,
        int resultstride, const RtPoint *p, const RtPoint *dPdu,
        const RtPoint *dPdv, const RtPoint *dPdw) {
        for (int i = 0; i < neval; ++i) {
            GetVertexValueFiltered(result, *p++, *dPdu++, *dPdv++, *dPdw++);
            result += resultstride;
        }
    }    
};

class ImplicitField{
public:

    /// The bbox field must be filled in during initialization with a
    /// bounding box outside which the field value is guaranteed to be
    /// identically zero at shutter open time, in the object
    /// coordinate system active at the time the geometry is created).
    RtBound bbox;

private:
    /* inhibit copying */
    ImplicitField(const ImplicitField &);
    ImplicitField &operator=(const ImplicitField &);

public:
    ImplicitField(){}
    virtual ~ImplicitField(){}

    /// Returns the field value at point p, in object coordinates, at
    /// shutter open time.
    virtual RtFloat Eval(const RtPoint p) = 0;

    /// Returns the filtered field value over the region of space in
    /// object coordinates delineated by point p and the three vectors
    /// dPdu, dPdv, and dPdw, at shutter open time.
    virtual RtFloat EvalFiltered(const RtPoint p, const RtPoint dPdu,
        const RtPoint dPdv, const RtPoint dPdw) {
        return Eval(p);
    }

    /// Calculates the neval field values at point p, in object
    /// coordinates, at shutter open time, and stores them in
    /// result. The DSO is required to step by resultstride when
    /// storing values in result.
    virtual void EvalMultiple(int neval, float *result, int resultstride,
        const RtPoint *p) {
        for (int i = 0; i < neval; ++i) {
            *result = Eval(*p++);
            result += resultstride;
        }
    }

    /// Calculates the neval filtered field values at point p, in
    /// object coordinates, at shutter open time, and stores them in
    /// result. The DSO is required to step by resultstride when
    /// storing values in result.
    virtual void EvalMultipleFiltered(int neval, float *result,
        int resultstride, const RtPoint *p, const RtPoint *dPdu,
        const RtPoint *dPdv, const RtPoint *dPdw) {
        EvalMultiple(neval, result, resultstride, p);
    }

    /// Stores the field gradient at point p, in object coordinates
    /// calculated at shutter open time, into result.
    virtual void GradientEval(RtPoint result, const RtPoint p) = 0;

    /// Stores the field gradient at point p, in object coordinates
    /// calculated at shutter open time, into result.
    virtual void GradientEvalFiltered(RtPoint result, const RtPoint p,
        const RtPoint dPdu, const RtPoint dPdv, const RtPoint dPdw) {
        GradientEval(result, p);
    }

    /// Calculates the field gradients at points p, in object
    /// coordinates calculated at shutter open time, and stores them
    /// in result.
    virtual void GradientEvalMultiple(int neval, RtPoint *result,
        const RtPoint *p) {
        for (int i = 0; i < neval; ++i) {
            GradientEval(*result++, *p++);
        }
    }

    /// Calculates the filtered field gradients at points p, in object
    /// coordinates calculated at shutter open time, and stores them
    /// in result.
    virtual void GradientEvalMultipleFiltered(int neval, RtPoint *result,
        const RtPoint *p, const RtPoint *dPdu, const RtPoint *dPdv,
        const RtPoint *dPdw) {
        GradientEvalMultiple(neval, result, p);
    }

    /// Stores into result the bounds of the field function values
    /// inside the region of object space with the given corners at
    /// shutter open.  While implementing this method is optional, it
    /// can provide an accurate understanding of the underlying field
    /// function to PRMan, and can have a dramatic impact on execution
    /// speed.  The default implementation results in exhaustive
    /// evaluation of the region.  The volume handle h uniquely
    /// identifies the volume, and is the same value that will later
    /// be passed to a call of VolumeCompleted.
    virtual void Range(RtInterval result, const RtPoint corners[8],
        const RtVolumeHandle h){
        result[0] = -1e30f;
        result[1] = 1e30f;
    }

    /// Returns true if the DSO requires splitting into children DSOs.
    virtual bool ShouldSplit() {
        return false;
    }

    /// Returns a list of children DSOs. Implementors should populate
    /// the children vector with a list of ImplicitField DSOs
    /// allocated with new; the renderer will be responsible for
    /// calling delete. Children DSOs should have bounding boxes that
    /// are entirely within the parent.
    virtual void Split(std::vector<ImplicitField *> &children) {
    }
    
    /// Stores into result how much the point p moves per unit time 
    /// (i.e. a motion vector) in object space. The default
    /// implementation assumes no motion and sets the result to be
    /// (0, 0, 0).
    virtual void Motion(RtPoint result, const RtPoint p) {
        result[0] = 0.0f;
        result[1] = 0.0f;
        result[2] = 0.0f;
    }

    /// Stores into result how much the point p moves per unit time
    /// (i.e. a motion vector) in object space, filtered over the
    /// region of space delineated by p and the three vectors dPdu,
    /// dPdv, and dPdw.
    virtual void MotionFiltered(RtPoint result, const RtPoint p,
        const RtPoint dPdu, const RtPoint dPdv, const RtPoint dPdw) {
        Motion(result, p);
    }

    /// Stores into result how much the neval points p move per unit
    /// time (i.e. motion vectors) in object space.
    virtual void MotionMultiple(int neval, RtPoint *result, const RtPoint *p) {
        for (int i = 0; i < neval; ++i) {
            Motion(*result++, *p++);
        }
    }

    /// Stores into result the filtered motion vectors representing
    /// the movement of the neval p values per unit time.
    virtual void MotionMultipleFiltered(int neval, RtPoint *result,
        const RtPoint *p, const RtPoint *dPdu, const RtPoint *dPdv,
        const RtPoint *dPdw) {
        MotionMultiple(neval, result, p);
    }

    /// Given a shutter open bounding box b, this routine must compute
    /// a corresponding bounding box taking into account any motion
    /// per unit time of the enclosed contents. That is: for
    /// any point p inside b, the result computed by BoxMotion() must
    /// include p plus the vector result computed by Motion() on p.
    virtual void BoxMotion(RtBound result, const RtBound b){
        for (int i = 0; i < 6; i++) {
            result[i] = b[i];
        }
    }

    /// VolumeCompleted is a courtesy callback, hinting that PRMan has
    /// finished processing all points inside the volume with the
    /// given handle, so that the plugin can discard data that it no
    /// longer needs. Using VolumeCompleted is a little tricky: PRMan
    /// calls Range with a particular RtVolumeHandle when it starts to
    /// work on a part of the level-set, and calls VolumeCompleted
    /// with the same handle when it's done. But it may in the interim
    /// have subdivided and called Range on smaller contained volumes
    /// in which it may maintain an interest after it has called
    /// VolumeCompleted on the parent volume. The handle passed to
    /// VolumeCompleted may be reused in a subsequent call to Range,
    /// but it will never ambiguously identify two volumes in which
    /// prman simultaneously maintains an interest.
    virtual void VolumeCompleted(const RtVolumeHandle h) {
    }

    /// CreateVertexValue informs the plugin of a vertex variable
    /// declaration, asking that the plugin provide PRMan with an
    /// entry point that evaluates the variable.  Arguments are the
    /// type and name of a vertex variable (e.g. "color Cs"), and the
    /// number of float components it has, 1 for scalars or 3 for
    /// point types.  The DSO is required to allocate (using C++'s new
    /// operator) and return an instance of a subclass of
    /// ImplicitVertexValue.  PRMan will call delete on the result
    /// when it is done with it.  If name is unknown to the plugin,
    /// the call should return NULL.
    virtual ImplicitVertexValue *CreateVertexValue(const RtToken name,
        int nvalue) {
        return 0;
    }

    /// This callback allows the plugin to hint at the minimum size of
    /// a voxel in object space as a way of avoiding potential
    /// overdicing. It will be called in PRMan if Attribute "dice"
    /// "float minlength" is set to -1.
    virtual float MinimumVoxelSize(const RtPoint corners[8]) {
        return 0.0f;
    }
};

#define	FIELDCREATE                                             \
    extern "C" const PRMANEXPORT int ImplicitFieldVersion=4;    \
    extern "C" PRMANEXPORT ImplicitField *ImplicitFieldNew(	\
        int nfloat,                                             \
            const RtFloat *float0, const RtFloat *float1,	\
            int nstring, const RtString *string)
#endif

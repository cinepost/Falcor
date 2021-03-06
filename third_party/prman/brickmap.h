#ifndef _BRICKMAP_H_
#define _BRICKMAP_H_
/* $Revision: #9 $  
 * (Pixar - RenderMan Division)
 * $Date: 2007/05/01 $ 
 */
/*
# ------------------------------------------------------------------------------
#
# Copyright (c) 2006-2010 Pixar Animation Studios. All rights reserved.
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

#include "prmanapi.h"

typedef void *BkmBrickMap;
typedef void *BkmBrickMapNode; // was unsigned int in API version 2

#ifdef __cplusplus
extern "C" {
#endif

/* Brick map API version number */
#define RMAN_BRICKMAP_API_VERSION 3

/* Brick map API */
PRMANAPI extern BkmBrickMap BkmOpenBrickMapFile(char *filename);
PRMANAPI extern int BkmGetBrickMapInfo(BkmBrickMap brickmap, char *request,
				       void *result);
PRMANAPI extern int BkmGetOctreeRoots(BkmBrickMap brickmap,
				      BkmBrickMapNode roots[7]);
PRMANAPI extern int BkmGetOctreeChildren(BkmBrickMapNode node,
					 BkmBrickMapNode children[8]);
PRMANAPI extern int BkmGetOctreeChildrenList(BkmBrickMap brickmap, // new param!
					     BkmBrickMapNode node,
					     BkmBrickMapNode children[8]);
PRMANAPI extern int BkmGetVoxelData(BkmBrickMap brickmap, BkmBrickMapNode node,
			   int imin, int jmin, int kmin,
			   int imax, int jmax, int kmax,
			   int *nvoxels, int *voxelnumbers,
			   int *voxelhasdata, // (redundant, but useful)
                           float *voxeldata);
PRMANAPI extern void BkmCloseBrickMapFile(BkmBrickMap brickmap);

#ifdef __cplusplus
}
#endif

#endif /* _BRICKMAP_H_ */

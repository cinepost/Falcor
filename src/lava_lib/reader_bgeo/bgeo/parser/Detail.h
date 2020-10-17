/*
 *  Copyright 2018 Laika, LLC. Authored by Peter Stuart
 *
 *  Licensed under the Apache License, Version 2.0, <LICENSE-APACHE or
 *  http://apache.org/licenses/LICENSE-2.0> or the MIT license <LICENSE-MIT or
 *  http://opensource.org/licenses/MIT>, at your option. This file may not be
 *  copied, modified, or distributed except according to those terms.
 */

#ifndef BGEO_PARSER_DETAIL_H
#define BGEO_PARSER_DETAIL_H

#include <memory>

//#include <UT/UT_JSONParser.h>
//#include <UT/UT_String.h>

#include "VertexMap.h"
#include "Primitives.h"
#include "FileVersion.h"
#include "types.h"

class UT_JSONParser;

namespace ika {
namespace bgeo {
namespace parser {

class Info;
class Attribute;
class PrimitiveGroup;

class Detail {
 public:
    Detail(bool checkVersion = true);
    ~Detail();

    void loadHeaderAndInfo(UT_JSONParser& parser);
    void loadGeometry(UT_JSONParser& parser);

    std::shared_ptr<Info> info() const { return mpInfo; };

    friend std::ostream& operator << (std::ostream& co, const Detail& detail);

    int64 getPointIndexForVertex(int64 vertex) const;
    const Attribute* getPointAttributeByName(const char* name) const;
    const Attribute* getVertexAttributeByName(const char* name) const;
    const Attribute* getPrimitiveAttributeByName(const char* name) const;
    const Attribute* getDetailAttributeByName(const char* name) const;

    const VertexMap& getVertexMap() const { return vertexMap; };

    void mapVerticesToPoints(const VertexArray& vertices, VertexArray& points) const;

    FileVersion fileVersion;
    int64 pointCount;
    int64 vertexCount;
    int64 primitiveCount;
    std::shared_ptr<Info> mpInfo = nullptr;
    VertexMap vertexMap;

    // FIXME: use vector of unique_ptrs instead
    std::vector<Attribute*> vertexAttributes;
    std::vector<Attribute*> pointAttributes;
    std::vector<Attribute*> primitiveAttributes;
    std::vector<Attribute*> detailAttributes;

    Primitives primitives;

    std::vector<std::unique_ptr<PrimitiveGroup>> primitiveGroups;

    bool checkVersion;
};

} // namespace parser
} // namespace bgeo
} // namespace ika

#endif // BGEO_PARSER_DETAIL_H

#include <osg2vsg/GeometryUtils.h>

namespace osg2vsg
{

    vsg::ref_ptr<vsg::vec2Array> convertToVsg(const osg::Vec2Array* inarray)
    {
        if (!inarray) return vsg::ref_ptr<vsg::vec2Array>();

        vsg::ref_ptr<vsg::vec2Array> outarray(new vsg::vec2Array(inarray->size()));
        for (unsigned int i = 0; i < inarray->size(); i++)
        {
            const osg::Vec2& osg2 = inarray->at(i);
            vsg::vec2 vsg2(osg2.x(), osg2.y());
            outarray->set(i, vsg2);
        }
        return outarray;
    }

    vsg::ref_ptr<vsg::vec3Array> convertToVsg(const osg::Vec3Array* inarray)
    {
        if (!inarray) return vsg::ref_ptr<vsg::vec3Array>();

        vsg::ref_ptr<vsg::vec3Array> outarray(new vsg::vec3Array(inarray->size()));
        for (unsigned int i = 0; i < inarray->size(); i++)
        {
            const osg::Vec3& osg3 = inarray->at(i);
            vsg::vec3 vsg3(osg3.x(), osg3.y(), osg3.z());
            outarray->set(i, vsg3);
        }
        return outarray;
    }

    vsg::ref_ptr<vsg::vec4Array> convertToVsg(const osg::Vec4Array* inarray)
    {
        if (!inarray) return vsg::ref_ptr<vsg::vec4Array>();

        vsg::ref_ptr<vsg::vec4Array> outarray(new vsg::vec4Array(inarray->size()));
        for (unsigned int i = 0; i < inarray->size(); i++)
        {
            const osg::Vec4& osg4 = inarray->at(i);
            vsg::vec4 vsg4(osg4.x(), osg4.y(), osg4.z(), osg4.w());
            outarray->set(i, vsg4);
        }
        return outarray;
    }

    vsg::ref_ptr<vsg::Data> convertToVsg(const osg::Array* inarray)
    {
        if (!inarray) return vsg::ref_ptr<vsg::Data>();

        switch (inarray->getType())
        {
            case osg::Array::Type::Vec2ArrayType: return convertToVsg(dynamic_cast<const osg::Vec2Array*>(inarray));
            case osg::Array::Type::Vec3ArrayType: return convertToVsg(dynamic_cast<const osg::Vec3Array*>(inarray));
            case osg::Array::Type::Vec4ArrayType: return convertToVsg(dynamic_cast<const osg::Vec4Array*>(inarray));
            default: return vsg::ref_ptr<vsg::Data>();
        }
    }

    uint32_t calculateAttributesMask(const osg::Geometry* geometry)
    {
        uint32_t mask = 0;
        if (geometry->getVertexArray() != nullptr) mask |= VERTEX;
        if (geometry->getNormalArray() != nullptr) mask |= NORMAL;
        if (geometry->getColorArray() != nullptr) mask |= COLOR;
        if (geometry->getTexCoordArray(0) != nullptr) mask |= TEXCOORD0;
        return mask;
    }

    vsg::ref_ptr<vsg::Geometry> convertToVsg(osg::Geometry* ingeometry, uint32_t requiredAttributesMask)
    {
        bool hasrequirements = requiredAttributesMask != 0;

        // convert attribute arrays, create defaults for any requested that don't exist for now to ensure pipline gets required data
        vsg::ref_ptr<vsg::Data> vertices(osg2vsg::convertToVsg(ingeometry->getVertexArray()));
        if (vertices->valueCount() == 0) return vsg::ref_ptr<vsg::Geometry>();

        unsigned int vertcount = vertices->valueCount();

        vsg::ref_ptr<vsg::Data> normals(osg2vsg::convertToVsg(ingeometry->getNormalArray()));
        if (normals->valueCount() == 0 && (requiredAttributesMask & NORMAL)) // if no normals but we've requested them, add them
        {
            vsg::ref_ptr<vsg::vec3Array> defaultnormals(new vsg::vec3Array(vertcount));
            for (unsigned int i = 0; i < vertcount; i++) defaultnormals->set(i, vsg::vec3(0.0f,1.0f,0.0f));
            normals = defaultnormals;
        }

        vsg::ref_ptr<vsg::Data> colors(osg2vsg::convertToVsg(ingeometry->getColorArray()));
        if (colors->valueCount() == 0 && (requiredAttributesMask & COLOR)) // if no colors but we've requested them, add them
        {
            vsg::ref_ptr<vsg::vec3Array> defaultcolors(new vsg::vec3Array(vertcount));
            for (unsigned int i = 0; i < vertcount; i++) defaultcolors->set(i, vsg::vec3(1.0f, 1.0f, 1.0f));
            colors = defaultcolors;
        }

        vsg::ref_ptr<vsg::Data> texcoord0(osg2vsg::convertToVsg(ingeometry->getTexCoordArray(0)));
        if (texcoord0->valueCount() == 0 && (requiredAttributesMask & TEXCOORD0)) // if no normals but we've requested them, add them
        {
            vsg::ref_ptr<vsg::vec2Array> defaulttex0(new vsg::vec2Array(vertcount));
            for (unsigned int i = 0; i < vertcount; i++) defaulttex0->set(i, vsg::vec2(0.0f, 0.0f));
            texcoord0 = defaulttex0;
        }

        // fill arrays data list
        auto attributeArrays = vsg::DataList{ vertices }; // always have verticies
        if (normals->valueCount() > 0 && (!hasrequirements || (requiredAttributesMask & NORMAL))) attributeArrays.push_back(normals);
        if (colors->valueCount() > 0 && (!hasrequirements || (requiredAttributesMask & COLOR))) attributeArrays.push_back(colors);
        if (texcoord0->valueCount() > 0 && (!hasrequirements || (requiredAttributesMask & TEXCOORD0))) attributeArrays.push_back(texcoord0);

        // convert indicies
        osg::Geometry::DrawElementsList drawElementsList;
        ingeometry->getDrawElementsList(drawElementsList);

        // only support first for now
        if (drawElementsList.size() == 0) return vsg::ref_ptr<vsg::Geometry>();

        osg::DrawElements* osgindices = drawElementsList.at(0);
        unsigned int numindcies = osgindices->getNumIndices();

        vsg::ref_ptr<vsg::ushortArray> indices(new vsg::ushortArray(numindcies));
        for (unsigned int i = 0; i < numindcies; i++)
        {
            indices->set(i, osgindices->index(i));
        }

        // create the vsg geometry
        auto geometry = vsg::Geometry::create();

        geometry->_arrays = attributeArrays;
        geometry->_indices = indices;

        vsg::ref_ptr<vsg::DrawIndexed> drawIndexed = vsg::DrawIndexed::create(indices->valueCount(), 1, 0, 0, 0);
        geometry->_commands = vsg::Geometry::Commands{ drawIndexed };

        return geometry;
    }
}



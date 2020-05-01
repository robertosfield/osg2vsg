#include <osg2vsg/GeometryUtils.h>
#include <osg2vsg/ImageUtils.h>
#include <osg2vsg/ShaderUtils.h>

#include <osgUtil/MeshOptimizers>
#include <osgUtil/TangentSpaceGenerator>

namespace osg2vsg
{

    vsg::ref_ptr<vsg::vec2Array> convertToVsg(const osg::Vec2Array* inarray, uint32_t bindOverallPaddingCount)
    {
        if (!inarray) return vsg::ref_ptr<vsg::vec2Array>();

        uint32_t count = inarray->size();
        uint32_t targetSize = std::max(count, bindOverallPaddingCount);

        vsg::ref_ptr<vsg::vec2Array> outarray(new vsg::vec2Array(targetSize));
        uint32_t i=0;
        for(; i<count; ++i)
        {
            const osg::Vec2& in_value = inarray->at(i);
            outarray->at(i) = vsg::vec2(in_value.x(), in_value.y());
        }

        if (i<bindOverallPaddingCount)
        {
            auto last = outarray->at(count-1);
            for(; i<bindOverallPaddingCount; ++i)
            {
                outarray->at(i) = last;
            }
        }

        return outarray;
    }

    vsg::ref_ptr<vsg::vec3Array> convertToVsg(const osg::Vec3Array* inarray, uint32_t bindOverallPaddingCount)
    {
        if (!inarray || inarray->size()==0) return vsg::ref_ptr<vsg::vec3Array>();

        uint32_t count = inarray->size();
        uint32_t targetSize = std::max(count, bindOverallPaddingCount);

        vsg::ref_ptr<vsg::vec3Array> outarray(new vsg::vec3Array(targetSize));
        uint32_t i=0;
        for(; i<count; ++i)
        {
            const osg::Vec3& in_value = inarray->at(i);
            outarray->at(i) = vsg::vec3(in_value.x(), in_value.y(), in_value.z());
        }

        if (i<bindOverallPaddingCount)
        {
            auto last = outarray->at(count-1);
            for(; i<bindOverallPaddingCount; ++i)
            {
                outarray->at(i) = last;
            }
        }

        return outarray;
    }

    vsg::ref_ptr<vsg::vec4Array> convertToVsg(const osg::Vec4Array* inarray, uint32_t bindOverallPaddingCount)
    {
        if (!inarray) return vsg::ref_ptr<vsg::vec4Array>();

        uint32_t count = inarray->size();
        uint32_t targetSize = std::max(count, bindOverallPaddingCount);

        vsg::ref_ptr<vsg::vec4Array> outarray(new vsg::vec4Array(targetSize));
        uint32_t i=0;
        for(; i<count; ++i)
        {
            const osg::Vec4& in_value = inarray->at(i);
            outarray->at(i) = vsg::vec4(in_value.x(), in_value.y(), in_value.z(), in_value.w());
        }

        if (i<bindOverallPaddingCount)
        {
            auto last = outarray->at(count-1);
            for(; i<bindOverallPaddingCount; ++i)
            {
                outarray->at(i) = last;
            }
        }

        return outarray;
    }

    vsg::ref_ptr<vsg::Data> convertToVsg(const osg::Array* inarray, uint32_t bindOverallPaddingCount)
    {
        if (!inarray) return vsg::ref_ptr<vsg::Data>();

        switch (inarray->getType())
        {
            case osg::Array::Type::Vec2ArrayType: return convertToVsg(dynamic_cast<const osg::Vec2Array*>(inarray), bindOverallPaddingCount);
            case osg::Array::Type::Vec3ArrayType: return convertToVsg(dynamic_cast<const osg::Vec3Array*>(inarray), bindOverallPaddingCount);
            case osg::Array::Type::Vec4ArrayType: return convertToVsg(dynamic_cast<const osg::Vec4Array*>(inarray), bindOverallPaddingCount);
            default: return vsg::ref_ptr<vsg::Data>();
        }
    }

    uint32_t calculateAttributesMask(const osg::Geometry* geometry)
    {
        uint32_t mask = 0;
        if(!geometry) return mask;

        if (geometry->getVertexArray() != nullptr) mask |= VERTEX;

        if (geometry->getNormalArray() != nullptr)
        {
            mask |= NORMAL;
            if(geometry->getNormalBinding() == osg::Geometry::AttributeBinding::BIND_OVERALL) mask |= NORMAL_OVERALL;
        }

        if (geometry->getColorArray() != nullptr)
        {
            mask |= COLOR;
            if (geometry->getColorBinding() == osg::Geometry::AttributeBinding::BIND_OVERALL) mask |= COLOR_OVERALL;
        }

        if (geometry->getVertexAttribArray(6) != nullptr)
        {
            mask |= TANGENT;
            if ( geometry->getVertexAttribBinding(6) == osg::Geometry::AttributeBinding::BIND_OVERALL) mask |= TANGENT_OVERALL;
        }

        if (geometry->getVertexAttribArray(7) != nullptr)
        {
            mask |= TRANSLATE;
            if ( geometry->getVertexAttribBinding(7) == osg::Geometry::AttributeBinding::BIND_OVERALL) mask |= TRANSLATE_OVERALL;
        }

        if (geometry->getTexCoordArray(0) != nullptr) mask |= TEXCOORD0;
        if (geometry->getTexCoordArray(1) != nullptr) mask |= TEXCOORD1;
        if (geometry->getTexCoordArray(2) != nullptr) mask |= TEXCOORD2;
        return mask;
    }

    VkPrimitiveTopology convertToTopology(osg::PrimitiveSet::Mode primitiveMode)
    {
        switch (primitiveMode)
        {
            case osg::PrimitiveSet::Mode::POINTS: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case osg::PrimitiveSet::Mode::LINES: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case osg::PrimitiveSet::Mode::LINE_STRIP: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case osg::PrimitiveSet::Mode::TRIANGLES: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case osg::PrimitiveSet::Mode::TRIANGLE_STRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case osg::PrimitiveSet::Mode::TRIANGLE_FAN: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            case osg::PrimitiveSet::Mode::LINES_ADJACENCY: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
            case osg::PrimitiveSet::Mode::LINE_STRIP_ADJACENCY: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
            case osg::PrimitiveSet::Mode::TRIANGLES_ADJACENCY: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
            case osg::PrimitiveSet::Mode::TRIANGLE_STRIP_ADJACENCY: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
            case osg::PrimitiveSet::Mode::PATCHES: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

            //not supported
            case osg::PrimitiveSet::Mode::LINE_LOOP:
            case osg::PrimitiveSet::Mode::QUADS:
            case osg::PrimitiveSet::Mode::QUAD_STRIP:
            case osg::PrimitiveSet::Mode::POLYGON:
            default: return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM; // use as unsupported flag`
        }
    }

    VkSamplerAddressMode covertToSamplerAddressMode(osg::Texture::WrapMode wrapmode)
    {
        switch (wrapmode)
        {
            case osg::Texture::WrapMode::CLAMP: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case osg::Texture::WrapMode::CLAMP_TO_EDGE: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case osg::Texture::WrapMode::CLAMP_TO_BORDER: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            case osg::Texture::WrapMode::REPEAT: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case osg::Texture::WrapMode::MIRROR: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
                //VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE no osg equivelent for this
            default: return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM; // unknown
        }
    }

    std::pair<VkFilter, VkSamplerMipmapMode> convertToFilterAndMipmapMode(osg::Texture::FilterMode filtermode)
    {
        switch (filtermode)
        {
            case osg::Texture::FilterMode::LINEAR: return { VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST };
            case osg::Texture::FilterMode::LINEAR_MIPMAP_LINEAR: return { VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR };
            case osg::Texture::FilterMode::LINEAR_MIPMAP_NEAREST: return { VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST };
            case osg::Texture::FilterMode::NEAREST: return { VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST };
            case osg::Texture::FilterMode::NEAREST_MIPMAP_LINEAR: return { VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_LINEAR };
            case osg::Texture::FilterMode::NEAREST_MIPMAP_NEAREST: return { VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST };
            default: return { VK_FILTER_MAX_ENUM, VK_SAMPLER_MIPMAP_MODE_MAX_ENUM }; // unknown
        }
    }

    VkSamplerCreateInfo convertToSamplerCreateInfo(const osg::Texture* texture)
    {
        auto minFilter = texture->getFilter(osg::Texture::MIN_FILTER);
        auto magFilter = texture->getFilter(osg::Texture::MAG_FILTER);
        auto minFilterMipmapMode = convertToFilterAndMipmapMode(minFilter);
        auto magFilterMipmapMode = convertToFilterAndMipmapMode(magFilter);
        bool mipmappingRequired = (minFilter!=osg::Texture::NEAREST) && (minFilter!=osg::Texture::LINEAR);

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.minFilter = minFilterMipmapMode.first;
        samplerInfo.magFilter = magFilterMipmapMode.first;
        samplerInfo.mipmapMode = minFilterMipmapMode.second;
        samplerInfo.addressModeU = covertToSamplerAddressMode(texture->getWrap(osg::Texture::WrapParameter::WRAP_S));
        samplerInfo.addressModeV = covertToSamplerAddressMode(texture->getWrap(osg::Texture::WrapParameter::WRAP_T));
        samplerInfo.addressModeW = covertToSamplerAddressMode(texture->getWrap(osg::Texture::WrapParameter::WRAP_R));

        // requres Logical device to have deviceFeatures.samplerAnisotropy = VK_TRUE; set when creating the vsg::Deivce
        samplerInfo.anisotropyEnable = texture->getMaxAnisotropy()>1.0f ? VK_TRUE : VK_FALSE;
        samplerInfo.maxAnisotropy = texture->getMaxAnisotropy();

        if (mipmappingRequired)
        {
            const osg::Image* image = (texture->getNumImages()>0) ? texture->getImage(0) : nullptr;

            auto maxDimension = std::max({image->s(), image->t(), image->r()});
            auto numMipMapLevels = static_cast<uint32_t>(std::floor(std::log2(maxDimension)))+1;

            samplerInfo.minLod = 0;
            samplerInfo.maxLod = static_cast<float>(numMipMapLevels);
            samplerInfo.mipLodBias = 0;
        }
        else
        {
            samplerInfo.minLod = 0;
            samplerInfo.maxLod = 0;
            samplerInfo.mipLodBias = 0;
        }

        // Vulkan doesn't supoort a vec4 border colour so have to map across to the enum's based on a best fit.
        osg::Vec4 borderColor = texture->getBorderColor();
        if (borderColor.a() < 0.5f)
        {
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        }
        else
        {
            bool nearerWhite = (borderColor.r() + borderColor.g() + borderColor.b()) >= 1.5f;
            samplerInfo.borderColor = nearerWhite ? VK_BORDER_COLOR_INT_OPAQUE_WHITE : VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        }

        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;

        return samplerInfo;
    }

    vsg::ref_ptr<vsg::materialValue> convertToMaterialValue(const osg::Material* material)
    {
        auto matvalue = vsg::materialValue::create();

        osg::Vec4 ambient = material->getAmbient(osg::Material::Face::FRONT);
        if (ambient.x() == 1.0f && ambient.y() == 1.0f && ambient.z() == 1.0f) ambient.set(0.1f, 0.1f, 0.1f, 1.0f); // ambient sanity check (shouldn't be needed but models seem to have bad material values)
        matvalue->value().ambientColor = vsg::vec4(ambient.x(), ambient.y(), ambient.z(), ambient.w());

        osg::Vec4 diffuse = material->getDiffuse(osg::Material::Face::FRONT);
        if(diffuse.x() == 0.0f && diffuse.y() == 0.0f && diffuse.z() == 0.0f) diffuse.set(1.0f,1.0f,1.0f,1.0f); // diffuse sanity check (shouldn't be needed but models seem to have bad material values)
        matvalue->value().diffuseColor = vsg::vec4(diffuse.x(), diffuse.y(), diffuse.z(), diffuse.w());

        osg::Vec4 specular = material->getSpecular(osg::Material::Face::FRONT);
        matvalue->value().specularColor = vsg::vec4(specular.x(), specular.y(), specular.z(), specular.w());

        float shininess = material->getShininess(osg::Material::Face::FRONT);
        matvalue->value().shininess = shininess;

        return matvalue;
    }

    vsg::ref_ptr<vsg::Command> convertToVsg(osg::Geometry* ingeometry, uint32_t requiredAttributesMask, GeometryTarget geometryTarget)
    {
        uint32_t instanceCount = 1;

        // work out if we need to enable instance by looking at BIND_OVERALL entries
        // to see if any have more than one element which we'll interpret requesting instancing, such as used for our custom osg::Billboard handling.
        {
            osg::Geometry::ArrayList arrays;
            if (ingeometry->getArrayList(arrays))
            {
                for(auto& array : arrays)
                {
                    if (array->getBinding()==osg::Array::BIND_OVERALL)
                    {
                        if (instanceCount < array->getNumElements()) instanceCount = array->getNumElements();
                    }
                }
            }

        }

        uint32_t bindOverallPaddingCount = instanceCount;


        // convert attribute arrays, create defaults for any requested that don't exist for now to ensure pipline gets required data
        vsg::ref_ptr<vsg::Data> vertices(osg2vsg::convertToVsg(ingeometry->getVertexArray(), bindOverallPaddingCount));
        if (!vertices.valid() || vertices->valueCount() == 0) return vsg::ref_ptr<vsg::Geometry>();

        // normals
        vsg::ref_ptr<vsg::Data> normals(osg2vsg::convertToVsg(ingeometry->getNormalArray(), bindOverallPaddingCount));

        // tangents
        vsg::ref_ptr<vsg::Data> tangents(osg2vsg::convertToVsg(ingeometry->getVertexAttribArray(6), bindOverallPaddingCount));
        if ((!tangents.valid() || tangents->valueCount() == 0) && (requiredAttributesMask & TANGENT))
        {
            osg::ref_ptr<osgUtil::TangentSpaceGenerator> tangentSpaceGenerator = new osgUtil::TangentSpaceGenerator();
            tangentSpaceGenerator->generate(ingeometry, 0);

            osg::Vec4Array* tangentArray = tangentSpaceGenerator->getTangentArray();
            //osg::Vec4Array* biNormalArray = tangGen->getBinormalArray();

            if (tangentArray && tangentArray->size() > 0)
            {
                tangents = osg2vsg::convertToVsg(tangentArray, bindOverallPaddingCount);
                // bind them to the osg geometry too??
                ingeometry->setVertexAttribArray(6, tangentArray);
                ingeometry->setVertexAttribBinding(6, osg::Geometry::BIND_PER_VERTEX);
            }
        }

        // colors
        vsg::ref_ptr<vsg::Data> colors(osg2vsg::convertToVsg(ingeometry->getColorArray(), bindOverallPaddingCount));

        // tex0
        vsg::ref_ptr<vsg::Data> texcoord0(osg2vsg::convertToVsg(ingeometry->getTexCoordArray(0), bindOverallPaddingCount));

        vsg::ref_ptr<vsg::Data> translations(osg2vsg::convertToVsg(ingeometry->getVertexAttribArray(7), bindOverallPaddingCount));

        // fill arrays data list THE ORDER HERE IS IMPORTANT
        auto attributeArrays = vsg::DataList{ vertices }; // always have verticies
        if (normals.valid() && normals->valueCount() > 0) attributeArrays.push_back(normals);
        if (tangents.valid() && tangents->valueCount() > 0) attributeArrays.push_back(tangents);
        if (colors.valid() && colors->valueCount() > 0) attributeArrays.push_back(colors);
        if (texcoord0.valid() && texcoord0->valueCount() > 0) attributeArrays.push_back(texcoord0);
        if (translations.valid() && translations->valueCount() > 0) attributeArrays.push_back(translations);

        // convert indicies

        // asume all the draw elements use the same primitive mode, copy all drawelements indicies into one indicie array and use in single drawindexed command
        // create a draw command per drawarrays primitive set

        vsg::Geometry::DrawCommands drawCommands;

        std::vector<uint16_t> indcies; // use to combine indicies from all drawelements
        osg::Geometry::PrimitiveSetList& primitiveSets = ingeometry->getPrimitiveSetList();
        for (osg::Geometry::PrimitiveSetList::const_iterator itr = primitiveSets.begin();
            itr != primitiveSets.end();
            ++itr)
        {
            osg::DrawElements* de = (*itr)->getDrawElements();
            if (de)
            {
                // merge indicies
                auto numindcies = de->getNumIndices();
                for (unsigned int i = 0; i < numindcies; i++)
                {
                    indcies.push_back(de->index(i));
                }
            }
            else
            {
                // see if we have a drawarrays and create a draw command
                if ((*itr)->getType() == osg::PrimitiveSet::Type::DrawArraysPrimitiveType) //  asDrawArrays != nullptr)
                {
                    osg::DrawArrays* da = dynamic_cast<osg::DrawArrays*>((*itr).get());

                    drawCommands.push_back(vsg::Draw::create(da->getCount(), instanceCount, da->getFirst(), 0));
                }
            }
        }

        vsg::ref_ptr<vsg::ushortArray> vsgindices;
        if(indcies.size() > 0)
        {
            vsgindices = new vsg::ushortArray(indcies.size());
            std::copy(indcies.begin(), indcies.end(), reinterpret_cast<uint16_t*>(vsgindices->dataPointer()));
        }

        if (geometryTarget == VSG_COMMANDS)
        {
            vsg::ref_ptr<vsg::Commands> commands(new vsg::Commands);

            commands->addChild( vsg::BindVertexBuffers::create(0, attributeArrays) );

            for(auto& draw : drawCommands)
            {
                commands->addChild(draw);
            }

            if(vsgindices)
            {
                commands->addChild( vsg::BindIndexBuffer::create(vsgindices) );
                commands->addChild( vsg::DrawIndexed::create(vsgindices->valueCount(), instanceCount, 0, 0, 0) );
            }

            return commands;
        }
        else if (geometryTarget == VSG_VERTEXINDEXDRAW && vsgindices && drawCommands.empty())
        {
            vsg::ref_ptr<vsg::VertexIndexDraw> vid(new vsg::VertexIndexDraw());

            vid->arrays = attributeArrays;
            vid->indices = vsgindices;
            vid->indexCount = vsgindices->size();
            vid->instanceCount = instanceCount;
            vid->firstIndex = 0;
            vid->vertexOffset = 0;
            vid->firstInstance = 0;

            return vid;
        }


        // fallback to create the vsg geometry
        auto geometry = vsg::Geometry::create();

        geometry->arrays = attributeArrays;

        // copy into ushortArray
        if(indcies.size() > 0)
        {
            geometry->indices = vsgindices;

            drawCommands.push_back(vsg::DrawIndexed::create(vsgindices->valueCount(), instanceCount, 0, 0, 0));
        }

        geometry->commands = drawCommands;

        return geometry;
    }

}


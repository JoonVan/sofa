/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CONTACT_LISTENER_H
#define SOFA_CONTACT_LISTENER_H
#include "config.h"

#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/collision/Contact.h>

namespace sofa
{
namespace core
{

// forward declaration
class CollisionModel;

namespace collision
{

// forward declaration
class NarrowPhaseDetection;

class SOFA_BASE_COLLISION_API ContactListener : public virtual core::objectmodel::BaseObject
{
public:
    SOFA_ABSTRACT_CLASS(ContactListener, core::objectmodel::BaseObject);


    ContactListener( CollisionModel* collModel1 = nullptr, CollisionModel* collModel2 = nullptr );
    ~ContactListener() override ;

    void init(void) override;

    // DetectionOutput iterators
    typedef helper::vector<const helper::vector<DetectionOutput>* >::const_iterator ContactVectorsIterator;
    typedef helper::vector<DetectionOutput>::const_iterator ContactsIterator;

    virtual void beginContact(const helper::vector<const helper::vector<DetectionOutput>* >& ) {}
    virtual void endContact(void*) {}

    void handleEvent( core::objectmodel::Event* event ) override;

    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        core::CollisionModel* collModel1 = nullptr;
        core::CollisionModel* collModel2 = nullptr;

        std::string collModelPath1;
        std::string collModelPath2;

        if (arg->getAttribute("collisionModel1"))
            collModelPath1 = arg->getAttribute("collisionModel1");
        else
            collModelPath1 = "";

        context->findLinkDest(collModel1, collModelPath1, nullptr);

        if (arg->getAttribute("collisionModel2"))
            collModelPath2 = arg->getAttribute("collisionModel2");
        else
            collModelPath2 = "";

        context->findLinkDest(collModel2, collModelPath2, nullptr);

        if (collModel1 == nullptr && collModel2 == nullptr )
        {
            arg->logError("Data attributes 'collisionModel1' and 'collisionModel2' are not pointing to valid collision models.");
            return false;
        }

        return BaseObject::canCreate(obj, context, arg);
    }

    template<class T>
    static typename T::SPtr create(T* , core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        CollisionModel* collModel1 = nullptr;
        CollisionModel* collModel2 = nullptr;

        std::string collModelPath1;
        std::string collModelPath2;

        if(arg)
        {
            collModelPath1 = arg->getAttribute(std::string("collisionModel1"), nullptr );
            collModelPath2 = arg->getAttribute(std::string("collisionModel2"), nullptr );

            // now 3 cases
            if ( strcmp( collModelPath1.c_str(),"" ) != 0  )
            {
                context->findLinkDest(collModel1, collModelPath1, nullptr);

                if ( strcmp( collModelPath2.c_str(),"" ) != 0 )
                {
                    context->findLinkDest(collModel2, collModelPath2, nullptr);
                }
            }
            else
            {
                context->findLinkDest(collModel1, collModelPath2, nullptr);
            }
        }
        typename T::SPtr obj = sofa::core::objectmodel::New<T>( collModel1, collModel2 );

        if (context)
        {
            context->addObject(obj);
        }

        if (arg)
        {
            obj->parse(arg);
        }

        return obj;
    }

protected:
    const CollisionModel* m_CollisionModel1;
    const CollisionModel* m_CollisionModel2;

private:
    helper::vector<const helper::vector<DetectionOutput>* > m_ContactsVector;
    core::collision::NarrowPhaseDetection* m_NarrowPhase;
};

} // namespace collision

} // namespace core

} // namespace sofa

#endif // SOFA_CONTACT_LISTENER_H

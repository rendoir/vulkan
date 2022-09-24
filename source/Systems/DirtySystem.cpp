#include <Systems/DirtySystem.hpp>

void DirtySystem::Update()
{
    UpdateDirtyComponents( DirtiableComponentsTypeList{} );
    UpdateDirtyComponents( GlobalDirtiableComponentsTypeList{} );
}

// MONGODB MODIFCATION - disable DLL hook
// gperftools/windows/patch_functions.cc normally defines this function,
// but we do not link this file since it would dynamically patch our functions.
// We override the behavior of this function to no-patch functions, but instead
// simply to do nothing
// TCMalloc calls this via a static initializer
void PatchWindowsFunctions() {
    // Intentionally left empty
}

/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

package com.spacelion.atlas_builder.build_target

import java.io.File

abstract class BuildTargetBase(val name : String,
                      val rootDirectory : File,
                      val options : Set<String>,
                      val oneValueArgs : Map<String, String>,
                      val multiValueArgs : Map<String, List<String>>) {

    private val publicLinkTargets : MutableSet<BuildTargetBase> = mutableSetOf()
    private val privateLinkTargets : MutableSet<BuildTargetBase> = mutableSetOf()

    abstract fun exportSymbol() : Boolean

    fun getPublicLinkTargets() : Set<BuildTargetBase> = publicLinkTargets
    fun getPrivateLinkTargets() : Set<BuildTargetBase> = privateLinkTargets
    fun addPublicLinkTarget(target: BuildTargetBase)  = publicLinkTargets.add(target)
    fun addPrivateLinkTarget(target: BuildTargetBase) = privateLinkTargets.add(target)

    open fun getDependenciesBuildTargets(dependencies : MutableSet<BuildTargetBase>) {
        for (target in publicLinkTargets) {
            target.getDependenciesBuildTargets(dependencies)
            dependencies.add(target)
        }

        for (target in privateLinkTargets) {
            target.getDependenciesBuildTargets(dependencies)
            dependencies.add(target)
        }
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false
        check(other is BuildTargetBase)
        return name == other.name
    }

    override fun hashCode(): Int {
        return name.hashCode()
    }
}
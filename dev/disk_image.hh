/*
 * Copyright (c) 2003 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* @file
 * Disk Image Interfaces
 */

#ifndef __DISK_IMAGE_HH__
#define __DISK_IMAGE_HH__

#include <fstream>

#include "base/hashmap.hh"
#include "sim/sim_object.hh"

#define SectorSize (512)

/*
 * Basic interface for accessing a disk image.
 */
class DiskImage : public SimObject
{
  protected:
    bool initialized;

  public:
    DiskImage(const std::string &name) : SimObject(name), initialized(false) {}
    virtual ~DiskImage() {}

    virtual off_t size() const = 0;

    virtual off_t read(uint8_t *data, off_t offset) const = 0;
    virtual off_t write(const uint8_t *data, off_t offset) = 0;
};

/*
 * Specialization for accessing a raw disk image
 */
class RawDiskImage : public DiskImage
{
  protected:
    mutable std::fstream stream;
    std::string file;
    bool readonly;
    mutable off_t disk_size;

  public:
    RawDiskImage(const std::string &name, const std::string &filename,
                 bool rd_only);
    ~RawDiskImage();

    void close();
    void open(const std::string &filename, bool rd_only = false);

    virtual off_t size() const;

    virtual off_t read(uint8_t *data, off_t offset) const;
    virtual off_t write(const uint8_t *data, off_t offset);
};

/*
 * Specialization for accessing a copy-on-write disk image layer.
 * A copy-on-write(COW) layer must be stacked on top of another disk
 * image layer this layer can be another CowDiskImage, or a
 * RawDiskImage.
 *
 * This object is designed to provide a mechanism for persistant
 * changes to a main disk image, or to provide a place for temporary
 * changes to the image to take place that later may be thrown away.
 */
class CowDiskImage : public DiskImage
{
  public:
    static const int VersionMajor;
    static const int VersionMinor;

  protected:
    struct Sector {
        uint8_t data[SectorSize];
    };
    typedef m5::hash_map<uint64_t, Sector *> SectorTable;

  protected:
    std::string filename;
    DiskImage *child;
    SectorTable *table;

  public:
    CowDiskImage(const std::string &name, DiskImage *kid, int hash_size);
    CowDiskImage(const std::string &name, DiskImage *kid, int hash_size,
                 const std::string &filename, bool read_only);
    ~CowDiskImage();

    void init(int hash_size);
    bool open();
    void save();
    void writeback();

    virtual off_t size() const;

    virtual off_t read(uint8_t *data, off_t offset) const;
    virtual off_t write(const uint8_t *data, off_t offset);
};

#endif // __DISK_IMAGE_HH__

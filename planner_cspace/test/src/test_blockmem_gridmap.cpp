/*
 * Copyright (c) 2017, the neonavigation authors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the names of its 
 *       contributors may be used to endorse or promote products derived from 
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstddef>

#include <gtest/gtest.h>

#include <blockmem_gridmap.h>

TEST(BlockmemGridmapTest, testResetClear)
{
  BlockMemGridmap<float, 3, 3, 0x20> gm;

  for (int s = 4; s <= 6; s += 2)
  {
    const int size[3] =
    {
      s, s, s
    };
    gm.reset(CyclicVecInt<3, 3>(size));
    gm.clear(0.0);

    CyclicVecInt<3, 3> i;
    for (i[0] = 0; i[0] < size[0]; ++i[0])
    {
      for (i[1] = 0; i[1] < size[1]; ++i[1])
      {
        for (i[2] = 0; i[2] < size[2]; ++i[2])
        {
          ASSERT_EQ(gm[i], 0.0);
        }
      }
    }

    gm.clear(3.0);
    for (i[0] = 0; i[0] < size[0]; ++i[0])
    {
      for (i[1] = 0; i[1] < size[1]; ++i[1])
      {
        for (i[2] = 0; i[2] < size[2]; ++i[2])
        {
          ASSERT_EQ(gm[i], 3.0);
        }
      }
    }
  }
}

TEST(BlockmemGridmapTest, testWriteRead)
{
  BlockMemGridmap<float, 3, 3, 0x20> gm;

  const int size[3] =
  {
    4, 4, 4
  };
  gm.reset(CyclicVecInt<3, 3>(size));
  gm.clear(0.0);

  CyclicVecInt<3, 3> i;
  for (i[0] = 0; i[0] < size[0]; ++i[0])
  {
    for (i[1] = 0; i[1] < size[1]; ++i[1])
    {
      for (i[2] = 0; i[2] < size[2]; ++i[2])
      {
        gm[i] = i[2] * 100 + i[1] * 10 + i[0];
      }
    }
  }

  for (i[0] = 0; i[0] < size[0]; ++i[0])
  {
    for (i[1] = 0; i[1] < size[1]; ++i[1])
    {
      for (i[2] = 0; i[2] < size[2]; ++i[2])
      {
        ASSERT_EQ(gm[i], i[2] * 100 + i[1] * 10 + i[0]);
      }
    }
  }
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}

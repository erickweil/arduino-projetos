#include <UnitTest.h>

using namespace fakeit;

#include "PositionQueue.h"

void test_enqueue_and_size()
{
    Posicao p1 = {1, 123456789, -123456789, 10, 20, 30, 4};
    TEST_ASSERT_EQUAL_UINT32(0, PositionQueue.size());
    PositionQueue.enqueue(p1);
    TEST_ASSERT_EQUAL_UINT32(1, PositionQueue.size());
}

void test_dequeue_for_send_and_commit()
{
    // Ensure clean state
    Posicao p1 = {10, 100000000, 200000000, 5, 10, 20, 6};
    Posicao p2 = {11, 110000000, 210000000, 6, 11, 21, 7};

    PositionQueue.resetForUnitTest();
    PositionQueue.enqueue(p1);
    PositionQueue.enqueue(p2);
    TEST_ASSERT_EQUAL_UINT32(2, PositionQueue.size());

    PositionQueue.resetSend();
    Posicao out;
    TEST_ASSERT_TRUE(PositionQueue.dequeueForSend(out));
    TEST_ASSERT_EQUAL_UINT32(p1.timestamp, out.timestamp);

    // still size should be 2 until commitSend
    TEST_ASSERT_EQUAL_UINT32(2, PositionQueue.size());

    // simulate acknowledging the first
    TEST_ASSERT_TRUE(PositionQueue.commitSend());
    TEST_ASSERT_EQUAL_UINT32(1, PositionQueue.size());
}

void test_wrap_around_and_overwrite()
{
    PositionQueue.resetForUnitTest();

    const size_t cap = PositionQueue.capacity();
    // fill with capacity + 2 to force overwrite of oldest
    for (size_t i = 0; i < cap + 2; ++i)
    {
        Posicao p = { (uint32_t)(100 + i), (int32_t)i, (int32_t)i, 0,0,0,0 };
        PositionQueue.enqueue(p);
    }

    // size should be capacity (since we overwrite oldest)
    TEST_ASSERT_EQUAL_UINT32(cap, PositionQueue.size());

    // dequeue all via send window
    PositionQueue.resetSend();
    Posicao out;
    size_t count = 0;
    while (PositionQueue.dequeueForSend(out)) {
        (void)out;
        ++count;
    }
    TEST_ASSERT_EQUAL_UINT32(cap, count);
}

void test_commit_send_behavior()
{
    PositionQueue.resetForUnitTest();
    // empty queue: commitSend should be false
    TEST_ASSERT_FALSE(PositionQueue.commitSend());

    // enqueue one position
    Posicao p = {42, 1, 2, 0,0,0,0};
    PositionQueue.enqueue(p);
    TEST_ASSERT_EQUAL_UINT32(1, PositionQueue.size());

    // resetSend sets send index to start; commitSend still false
    PositionQueue.resetSend();
    TEST_ASSERT_FALSE(PositionQueue.commitSend());

    // dequeue one for send -> advances send index
    Posicao out;
    TEST_ASSERT_TRUE(PositionQueue.dequeueForSend(out));
    TEST_ASSERT_EQUAL_UINT32(p.timestamp, out.timestamp);

    // now commitSend should confirm and reduce size
    TEST_ASSERT_TRUE(PositionQueue.commitSend());
    TEST_ASSERT_EQUAL_UINT32(0, PositionQueue.size());
}

void test_overwrite_content()
{
    PositionQueue.resetForUnitTest();
    const size_t cap = PositionQueue.capacity();
    // enqueue values 1..(cap+3)
    for (size_t i = 1; i <= cap + 3; ++i)
    {
        Posicao p = { (uint32_t)i, (int32_t)i, (int32_t)i, 0,0,0,0 };
        PositionQueue.enqueue(p);
    }

    // After overwriting, the queue should contain the last `cap` timestamps: (4..cap+3)
    PositionQueue.resetSend();
    Posicao out;
    size_t idx = 0;
    while (PositionQueue.dequeueForSend(out)) {
        uint32_t expected = (uint32_t)(4 + idx);
        TEST_ASSERT_EQUAL_UINT32(expected, out.timestamp);
        ++idx;
    }
    TEST_ASSERT_EQUAL_UINT32(cap, idx);
}

void test_position_queue()
{
    RUN_TEST(test_enqueue_and_size);
    RUN_TEST(test_dequeue_for_send_and_commit);
    RUN_TEST(test_wrap_around_and_overwrite);
    RUN_TEST(test_commit_send_behavior);
    RUN_TEST(test_overwrite_content);
}

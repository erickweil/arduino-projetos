#include <UnitTest.h>

using namespace fakeit;

//# include "PositionQueue.h"

void run_callbacks_on_both_impls(void (*callback)(PositionQueueClass &PositionQueue))
{
    PositionQueueRTC qRTC;
    callback(qRTC);

    PositionQueueLittleFS qLFS;
    qLFS.resetForUnitTest();
    callback(qLFS);
}

void test_enqueue_and_size()
{
    // below is inline function callback that accepts PositionQueueClass interface
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
        TEST_ASSERT_EQUAL_UINT32(0, PositionQueue.size());
        TEST_ASSERT_TRUE(PositionQueue.isEmpty() == true);

        Posicao p1 = {1, 0, 0, 0, 0, 0, 0};
        PositionQueue.enqueue(p1);
        TEST_ASSERT_EQUAL_UINT32(1, PositionQueue.size());
        TEST_ASSERT_TRUE(PositionQueue.isEmpty() == false);

        Posicao p2 = {2, 0, 0, 0, 0, 0, 0};
        PositionQueue.enqueue(p2);
        TEST_ASSERT_EQUAL_UINT32(2, PositionQueue.size());
        TEST_ASSERT_TRUE(PositionQueue.isEmpty() == false);
    });
}

void test_dequeue_for_send_and_commit()
{
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
        Posicao p1 = {10, 100000000, 200000000, 5, 10, 20, 6};
        Posicao p2 = {11, 110000000, 210000000, 6, 11, 21, 7};
        
        TEST_ASSERT_TRUE(PositionQueue.enqueue(p1));
        TEST_ASSERT_TRUE(PositionQueue.enqueue(p2));
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
    });
}

void test_commit_send_behavior()
{
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
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
    });
}

void test_overwrite_content()
{
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
        const size_t cap = PositionQueue.capacity();
        // enqueue values 1..(cap+10)
        for (size_t i = 1; i <= cap + 10; ++i)
        {
            Posicao p = { (uint32_t)i, (int32_t)i, (int32_t)i, 0,0,0,0 };
            PositionQueue.enqueue(p);
        }

        // After overwriting, the queue should contain at least half the values, but still counting up to (cap+3)
        PositionQueue.resetSend();
        Posicao out;
        TEST_ASSERT_TRUE(PositionQueue.dequeueForSend(out));
        uint32_t start = out.timestamp;
        uint32_t idx = 1;
        while (PositionQueue.dequeueForSend(out)) {
            TEST_ASSERT_EQUAL_UINT32(start + idx, out.timestamp);
            ++idx;
        }
        TEST_ASSERT_EQUAL_UINT32(cap+10, out.timestamp);
    });
}

void test_queue_get_at()
{
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
        const size_t cap = PositionQueue.capacity();
        // enqueue values 1..cap
        for (size_t i = 1; i <= cap; ++i)
        {
            Posicao p = { (uint32_t)i, (int32_t)i, (int32_t)i, 0,0,0,0 };
            PositionQueue.enqueue(p);
        }

        // Test getAt for all valid indices
        for (size_t i = 0; i < cap; ++i)
        {
            Posicao out;
            bool ok = PositionQueue.getAt(i, out);
            TEST_ASSERT_TRUE(ok);
            uint32_t expected = (uint32_t)(i + 1);
            TEST_ASSERT_EQUAL_UINT32(expected, out.timestamp);
        }

        // Test getAt for invalid index
        Posicao out;
        bool ok = PositionQueue.getAt(cap, out);
        TEST_ASSERT_FALSE(ok);
    });
}

void test_write_read_loop()
{
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
        // 1. write 10 positions
        // 2. read them back to be sent
        // 3. commit send
        // 4. repeat above steps 10000x

        const size_t iterations = 10000;
        const size_t positions_per_iteration = 10;
        uint32_t global_index = 1;
        for (size_t iter = 0; iter < iterations; ++iter)
        {
            // Write positions
            for (size_t i = 0; i < positions_per_iteration; ++i)
            {
                Posicao p = { global_index, (int32_t)global_index, (int32_t)global_index, 0,0,0,0 };
                PositionQueue.enqueue(p);
                global_index++;
            }

            // Read back positions
            PositionQueue.resetSend();
            Posicao out;
            for (size_t i = 0; i < positions_per_iteration; ++i)
            {
                TEST_ASSERT_TRUE(PositionQueue.dequeueForSend(out));
                uint32_t expected_index = (uint32_t)(iter * positions_per_iteration + i + 1);
                TEST_ASSERT_EQUAL_UINT32(expected_index, out.timestamp);
            }

            // Commit send
            TEST_ASSERT_TRUE(PositionQueue.commitSend());
            TEST_ASSERT_EQUAL_UINT32(0, PositionQueue.size());
        }
    });

    // Test loading again LittleFS from half written files
    PositionQueueLittleFS qLFS;
    qLFS.begin();

    // Por enquanto, vai ser assim mesmo, o último commit não é salvo
    TEST_ASSERT_EQUAL_UINT32(10, qLFS.size());
}

void test_to_json()
{
    Posicao p = {0, 0, 0, 3, 0, 11, 6};
    char buf[256];
    int ret = PositionQueueClass::toJson(p, 1, buf, sizeof(buf));
    TEST_ASSERT_TRUE(ret > 0);
    const char *expected = "{\"n\":1,\"data\":\"1970-01-01T00:00:00Z\",\"coords\":[0.0000000,0.0000000],\"vel\":3,\"dir\":0.00,\"hdop\":1.10,\"satellites\":6}";
    TEST_ASSERT_EQUAL_STRING(expected, buf);
}

void test_position_queue()
{
    RUN_TEST(test_enqueue_and_size);
    RUN_TEST(test_dequeue_for_send_and_commit);
    RUN_TEST(test_commit_send_behavior);
    RUN_TEST(test_overwrite_content);
    RUN_TEST(test_queue_get_at);
    RUN_TEST(test_write_read_loop);
    RUN_TEST(test_to_json);
}

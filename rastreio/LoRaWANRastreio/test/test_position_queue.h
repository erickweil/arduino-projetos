#include <UnitTest.h>

using namespace fakeit;

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

void test_commit_send_behavior()
{
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
        // 1. Enqueue 10 positions
        // 2. Read 10 positions for send
        // 3. Commit.
        // 4. Size should be 0

        const size_t num_positions = 10;
        for (size_t i = 0; i < num_positions; ++i)
        {
            Posicao p = { (uint32_t)(i + 1), (int32_t)(i + 1), (int32_t)(i + 1), 0,0,0,0 };
            TEST_ASSERT_TRUE(PositionQueue.enqueue(p));
            TEST_ASSERT_EQUAL_UINT32(i + 1, PositionQueue.size());
        }
        TEST_ASSERT_TRUE(PositionQueue.beginReadAt(PositionQueue.getSendIndex()));
        Posicao out;
        for (size_t i = 0; i < num_positions; ++i)
        {
            TEST_ASSERT_TRUE(PositionQueue.readNext(out));
            TEST_ASSERT_EQUAL_UINT32((uint32_t)(i + 1), out.timestamp);
        }
        size_t last = PositionQueue.endRead();
        TEST_ASSERT_EQUAL_UINT32(num_positions, last - PositionQueue.getSendIndex());
        PositionQueue.commitSend(last);
        TEST_ASSERT_EQUAL_UINT32(0, PositionQueue.pendingSend());
        TEST_ASSERT_EQUAL_UINT32(num_positions, PositionQueue.size());

    });
}

void test_overwrite_content()
{
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
        const size_t cap = PositionQueue.capacity();
        // enqueue values 1..(cap)
        for (size_t i = 1; i <= cap; ++i)
        {
            Posicao p = { (uint32_t)i, (int32_t)i, (int32_t)i, 0,0,0,0 };
            PositionQueue.enqueue(p);
        }

        // Test getAt for all valid indices
        TEST_ASSERT_TRUE(PositionQueue.beginReadAt(0));
        for (size_t i = 0; i < cap; ++i)
        {
            Posicao out;
            TEST_ASSERT_TRUE(PositionQueue.readNext(out));
            uint32_t expected = (uint32_t)(i + 1);
            TEST_ASSERT_EQUAL_UINT32(expected, out.timestamp);
        }
        // Test getAt for invalid index
        Posicao out;
        TEST_ASSERT_FALSE(PositionQueue.readNext(out));

        PositionQueue.endRead();

        for (size_t i = cap + 1; i <= cap + 10; ++i)
        {
            Posicao p = { (uint32_t)i, (int32_t)i, (int32_t)i, 0,0,0,0 };
            PositionQueue.enqueue(p);
        }

        // After overwriting, the queue should contain at least half the values, but still counting up to (cap+3)
        TEST_ASSERT_TRUE(PositionQueue.beginReadAt(PositionQueue.getSendIndex()));
        
        TEST_ASSERT_TRUE(PositionQueue.readNext(out));
        size_t count = 1;
        uint32_t start = out.timestamp;
        while (PositionQueue.readNext(out)) {
            TEST_ASSERT_EQUAL_UINT32(start + count, out.timestamp);
            ++count;
        }
        TEST_ASSERT_EQUAL_UINT32(cap + 10, out.timestamp);
        PositionQueue.endRead();
    });
}

void test_write_read_loop()
{
    run_callbacks_on_both_impls([](PositionQueueClass &PositionQueue) {
        // 1. write 10 positions
        // 2. read them back to be sent
        // 3. commit send
        // 4. repeat above steps 10000x

        const size_t cap = PositionQueue.capacity();

        const size_t positions_per_iteration = 10;
        const size_t iterations = (cap / positions_per_iteration) * 2; // write enough to wrap around
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

            TEST_ASSERT_TRUE(PositionQueue.beginReadAt(PositionQueue.getSendIndex()));
            Posicao out;
            for (size_t i = 0; i < positions_per_iteration; ++i)
            {
                TEST_ASSERT_TRUE(PositionQueue.readNext(out));
                uint32_t expected_index = (uint32_t)(iter * positions_per_iteration + i + 1);
                TEST_ASSERT_EQUAL_UINT32(expected_index, out.timestamp);
            }

            // Commit send
            size_t last = PositionQueue.endRead();
            PositionQueue.commitSend(last);
            TEST_ASSERT_EQUAL_UINT32(0, PositionQueue.pendingSend());
        }
    });

    // Test loading again LittleFS from half written files
    PositionQueueLittleFS qLFS;
    qLFS.begin();

    // Por enquanto, vai ser assim mesmo, o último commit não é salvo
    TEST_ASSERT_EQUAL_UINT32(10, qLFS.pendingSend());
    Serial.print("Size after reload:"); Serial.println(qLFS.size());
    TEST_ASSERT_TRUE(qLFS.size() >= (LITTLE_FS_QUEUE_FILES - 1) * (LITTLE_FS_QUEUE_MAX_FILE_SIZE / sizeof(Record)));
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
    RUN_TEST(test_commit_send_behavior);
    RUN_TEST(test_overwrite_content);
    RUN_TEST(test_write_read_loop);
    RUN_TEST(test_to_json);
}

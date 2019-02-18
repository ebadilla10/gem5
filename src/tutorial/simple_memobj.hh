#ifndef __TUTORIAL_SIMPLE_MEMOBJ_HH__
#define __TUTORIAL_SIMPLE_MEMOBJ_HH__

#include "mem/mem_object.hh"
#include "params/SimpleMemobj.hh"

class SimpleMemobj : public MemObject
{
  private:

    class CPUSidePort : public SlavePort
    {
      private:
        /// The object that owns this object (SimpleMemobj)
        SimpleMemobj *owner;

        /// True if the port needs to send a retry req.
        bool needRetry;

        /// If we tried to send a packet and it was blocked, store it here
        PacketPtr blockedPacket;

      public:

        CPUSidePort(const std::string& name, SimpleMemobj *owner) :
            SlavePort(name, owner), owner(owner), needRetry(false),
            blockedPacket(nullptr)
        { }


        void sendPacket(PacketPtr pkt);

        AddrRangeList getAddrRanges() const override;

        void trySendRetry();

      protected:
        Tick recvAtomic(PacketPtr pkt) override
        { panic("recvAtomic unimpl."); }

        void recvFunctional(PacketPtr pkt) override;

        bool recvTimingReq(PacketPtr pkt) override;

        void recvRespRetry() override;
    };

    class MemSidePort : public MasterPort
    {
      private:
        /// The object that owns this object (SimpleMemobj)
        SimpleMemobj *owner;

        /// If we tried to send a packet and it was blocked, store it here
        PacketPtr blockedPacket;

      public:
        MemSidePort(const std::string& name, SimpleMemobj *owner) :
            MasterPort(name, owner), owner(owner), blockedPacket(nullptr)
        { }

        void sendPacket(PacketPtr pkt);

      protected:
        bool recvTimingResp(PacketPtr pkt) override;

        void recvReqRetry() override;

        void recvRangeChange() override;
    };

    bool handleRequest(PacketPtr pkt);

    bool handleResponse(PacketPtr pkt);

    void handleFunctional(PacketPtr pkt);

    AddrRangeList getAddrRanges() const;

    void sendRangeChange();

    /// Instantiation of the CPU-side ports
    CPUSidePort instPort;
    CPUSidePort dataPort;

    /// Instantiation of the memory-side port
    MemSidePort memPort;

    /// True if this is currently blocked waiting for a response.
    bool blocked;

  public:

    SimpleMemobj(SimpleMemobjParams *params);

    BaseMasterPort& getMasterPort(const std::string& if_name,
                                  PortID idx = InvalidPortID) override;

    BaseSlavePort& getSlavePort(const std::string& if_name,
                                PortID idx = InvalidPortID) override;
};


#endif // __TUTORIAL_SIMPLE_MEMOBJ_HH__

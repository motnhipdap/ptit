import React, { useContext, useState } from 'react'
import Title from '../components/Title';
import NewsletterBox from '../components/NewsletterBox';
import { assets } from '../assets/assets';
import { ShopContext } from '../context/ShopContext';
import axios from 'axios';
import { toast } from 'react-toastify';
import { useNavigate } from 'react-router-dom';

const Contact = () => {
  const { backendUrl, token } = useContext(ShopContext);
  const navigate = useNavigate();
  const [showTicketForm, setShowTicketForm] = useState(false);
  const [formData, setFormData] = useState({
    productName: '',
    orderId: '',
    message: '',
    image: null
  });
  const [imagePreview, setImagePreview] = useState(null);
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleImageChange = (e) => {
    const file = e.target.files[0];
    if (file) {
      setFormData({ ...formData, image: file });
      // Create preview
      const reader = new FileReader();
      reader.onloadend = () => {
        setImagePreview(reader.result);
      };
      reader.readAsDataURL(file);
    }
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    
    if (!token) {
      toast.error('Please login to raise a support ticket');
      return;
    }

    if (!formData.productName || !formData.orderId || !formData.message || !formData.image) {
      toast.error('Please fill all fields and upload an image');
      return;
    }

    setIsSubmitting(true);

    try {
      const formDataToSend = new FormData();
      formDataToSend.append('productName', formData.productName);
      formDataToSend.append('orderId', formData.orderId);
      formDataToSend.append('message', formData.message);
      formDataToSend.append('image', formData.image);

      const response = await axios.post(
        backendUrl + '/api/tickets/create',
        formDataToSend,
        {
          headers: {
            'Content-Type': 'multipart/form-data',
            token
          }
        }
      );

      if (response.data.message) {
        toast.success('Support ticket created successfully!');
        // Reset form
        setFormData({
          productName: '',
          orderId: '',
          message: '',
          image: null
        });
        setImagePreview(null);
        setShowTicketForm(false);
        
        // Ask user if they want to view tickets
        setTimeout(() => {
          if (window.confirm('Ticket created! Would you like to view your tickets?')) {
            navigate('/my-tickets');
          }
        }, 500);
      }
    } catch (error) {
      console.error(error);
      toast.error(error.response?.data?.message || 'Failed to create ticket');
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <div>
      <div className='text-center text-2xl pt-10 border-t'>
        <Title text1={'CONTACT'} text2={'US'}/>
      </div>
      <div className='my-10 flex flex-col justify-center md:flex-row gap-10 mb-28'>
        <img className='w-full md:max-w-[480px]' src={assets.contact_img} alt="" />
        <div className='flex flex-col justify-center items-start gap-6'>
          <p className='font-semibold text-xl text-gray-600'>Our Store</p>
          <p className='text-gray-500'>54709 Willms Station <br /> Suite 350, Washington, USA </p>
          <p className='text-gray-500'>Tel: (415) 555-0132 <br /> Email: admin@X-Stop.com</p>
          <p className='font-semibold text-xl text-gray-600'>Careers at X-Stop</p>
          <p className='text-gray-600'>Learn more about our teams and job openings.</p>
          <button className='border border-black px-8 py-4 text-sm hover:bg-black hover:text-white transition-all duration-500'>Explore Jobs</button>
        </div>
      </div>

      {/* Support Ticket Section */}
      <div className='my-16 px-4'>
        <div className='text-center mb-8'>
          <Title text1={'CUSTOMER'} text2={'SUPPORT'}/>
          <p className='text-gray-600 mt-4 max-w-2xl mx-auto'>
            Need help with your order? Raise a support ticket and our team will assist you.
          </p>
        </div>

        {!showTicketForm ? (
          <div className='flex justify-center'>
            <button 
              onClick={() => setShowTicketForm(true)}
              className='bg-black text-white px-8 py-4 text-sm rounded hover:bg-gray-800 transition-all duration-500'
            >
              Raise Support Ticket
            </button>
          </div>
        ) : (
          <div className='max-w-2xl mx-auto bg-gray-50 p-8 rounded-lg shadow-md'>
            <form onSubmit={handleSubmit} className='space-y-6'>
              <div>
                <label className='block text-sm font-medium text-gray-700 mb-2'>
                  Product Name <span className='text-red-500'>*</span>
                </label>
                <input
                  type='text'
                  value={formData.productName}
                  onChange={(e) => setFormData({ ...formData, productName: e.target.value })}
                  className='w-full px-4 py-3 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-black'
                  placeholder='Enter product name'
                  required
                />
              </div>

              <div>
                <label className='block text-sm font-medium text-gray-700 mb-2'>
                  Order ID <span className='text-red-500'>*</span>
                </label>
                <input
                  type='text'
                  value={formData.orderId}
                  onChange={(e) => setFormData({ ...formData, orderId: e.target.value })}
                  className='w-full px-4 py-3 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-black'
                  placeholder='Enter your order ID'
                  required
                />
              </div>

              <div>
                <label className='block text-sm font-medium text-gray-700 mb-2'>
                  Issue Description <span className='text-red-500'>*</span>
                </label>
                <textarea
                  value={formData.message}
                  onChange={(e) => setFormData({ ...formData, message: e.target.value })}
                  rows='5'
                  className='w-full px-4 py-3 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-black resize-none'
                  placeholder='Describe your issue in detail...'
                  required
                />
              </div>

              <div>
                <label className='block text-sm font-medium text-gray-700 mb-2'>
                  Upload Image <span className='text-red-500'>*</span>
                </label>
                <div className='border-2 border-dashed border-gray-300 rounded-lg p-6 text-center hover:border-black transition-colors'>
                  {imagePreview ? (
                    <div className='space-y-4'>
                      <img src={imagePreview} alt='Preview' className='max-h-64 mx-auto rounded' />
                      <button
                        type='button'
                        onClick={() => {
                          setFormData({ ...formData, image: null });
                          setImagePreview(null);
                        }}
                        className='text-red-500 text-sm hover:underline'
                      >
                        Remove Image
                      </button>
                    </div>
                  ) : (
                    <div>
                      <label className='cursor-pointer'>
                        <div className='space-y-2'>
                          <div className='text-4xl'>📁</div>
                          <p className='text-gray-600'>Click to upload or drag and drop</p>
                          <p className='text-xs text-gray-500'>PNG, JPG, JPEG (MAX. 5MB)</p>
                        </div>
                        <input
                          type='file'
                          accept='image/*'
                          onChange={handleImageChange}
                          className='hidden'
                          required
                        />
                      </label>
                    </div>
                  )}
                </div>
              </div>

              <div className='flex gap-4 pt-4'>
                <button
                  type='submit'
                  disabled={isSubmitting}
                  className='flex-1 bg-black text-white py-3 rounded-md hover:bg-gray-800 transition-all duration-300 disabled:bg-gray-400 disabled:cursor-not-allowed'
                >
                  {isSubmitting ? 'Submitting...' : 'Submit Ticket'}
                </button>
                <button
                  type='button'
                  onClick={() => {
                    setShowTicketForm(false);
                    setFormData({ productName: '', orderId: '', message: '', image: null });
                    setImagePreview(null);
                  }}
                  className='flex-1 bg-gray-300 text-gray-700 py-3 rounded-md hover:bg-gray-400 transition-all duration-300'
                >
                  Cancel
                </button>
              </div>
            </form>
          </div>
        )}
      </div>

      <NewsletterBox/>
    </div>
  )
}

export default Contact;

import React from 'react'
import { assets } from '../assets/assets'

const Footer = () => {
  return (
    <div>
      <div className='flex flex-col sm:grid grid-cols-[3fr_1fr_1fr] gap-14 my-10 mt-40 text-sm'>
        <div>
          <div className="text-xl font-bold tracking-wide">
            X-Stop
          </div>
          <p className='w-full md:w-2/3 text-gray-600'>X-Stop is your one-stop destination for quality fashion and everyday essentials. We focus on affordable style, fast delivery, and a smooth shopping experience from cart to doorstep.</p>
        </div>
        <div>
          <p className='text-xl font-medium mb-5'>COMPANY</p>
          <ul className='flex flex-col gap-1 text-gray-600'>
            <li>Home</li>
            <li>About us</li>
            <li>Delivery</li>
            <li>Privacy Policy</li>
          </ul>
        </div>
        <div>
          <p className='text-xl font-medium mb-5'>GET IN TOUCH</p>
          <ul className='flex flex-col gap-1 text-gray-600'>
            <li>+91 98765 43210</li>
            <li>admin@xstop.com</li>
          </ul>
        </div>
        <div>
          <hr />
          <p className='py-5 text-sm text-center'>Copyright 2024@ X-Stop.com - All rights reserved.</p>
        </div>
      </div>
    </div>
  )
}

export default Footer;
